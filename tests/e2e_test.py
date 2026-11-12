#!/usr/bin/env python3
"""
End-to-end tests for arena_server: real UDP datagrams against a real server.

    python3 tests/e2e_test.py            # builds nothing, expects ./arena_server

Covers the mandatory points of the subject: protocol format, ACK, duplicates,
retransmission, authentication, input validation, combat, respawn (10 s),
session timeout, LOGOUT and packet loss.
"""
import os
import random
import re
import socket
import subprocess
import sys
import tempfile
import time

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SERVER = os.path.join(ROOT, "arena_server")
PORT = 47000 + random.randint(0, 999)
PASSED = 0


def check(condition, label):
    global PASSED
    if not condition:
        print(f"  FAIL  {label}")
        sys.exit(1)
    PASSED += 1
    print(f"  ok    {label}")


class Peer:
    """Minimal protocol client: sends with MSGID, ACKs server messages."""

    def __init__(self, port, auto_ack=True):
        self.addr = ("127.0.0.1", port)
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.settimeout(0.05)
        self.next_id = random.randint(1, 10_000_000)
        self.auto_ack = auto_ack
        self.inbox = []      # (msgid, type, fields)
        self.acks = set()

    def raw(self, text):
        self.sock.sendto(text.encode(), self.addr)

    def send(self, mtype, *fields, msg_id=None):
        if msg_id is None:
            msg_id = self.next_id
            self.next_id += 1
        self.raw("|".join([str(msg_id), mtype, *fields]))
        return msg_id

    def pump(self, duration):
        end = time.time() + duration
        while time.time() < end:
            try:
                data, _ = self.sock.recvfrom(4096)
            except socket.timeout:
                continue
            parts = data.decode().split("|")
            msg_id, mtype, fields = int(parts[0]), parts[1], parts[2:]
            if mtype == "ACK":
                self.acks.add(int(fields[0]))
                continue
            if msg_id != 0 and self.auto_ack:
                self.raw(f"0|ACK|{msg_id}")
            self.inbox.append((msg_id, mtype, fields))

    def expect(self, mtype, contains=None, timeout=4.0):
        end = time.time() + timeout
        while time.time() < end:
            for i, (msg_id, t, fields) in enumerate(self.inbox):
                if t == mtype and (contains is None or contains in "|".join(fields)):
                    return self.inbox.pop(i)
            self.pump(0.05)
        return None

    def request(self, mtype, *fields, reply="OK", contains=None, timeout=4.0):
        self.inbox.clear()
        self.send(mtype, *fields)
        return self.expect(reply, contains, timeout)

    def state(self):
        self.inbox = [m for m in self.inbox if m[1] != "STATE"]
        self.send("STATUS")
        msg = self.expect("STATE")
        return tuple(int(v) for v in msg[2]) if msg else None


def start_server(users, *extra):
    proc = subprocess.Popen([SERVER, "-p", str(PORT), "--users", users, *extra],
                            stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
    time.sleep(0.4)
    return proc


def main():
    if not os.path.exists(SERVER):
        print("arena_server not found — build the project first")
        return 1
    users = os.path.join(tempfile.mkdtemp(), "users.db")
    server = start_server(users, "--timeout", "15", "--seed", "7")
    try:
        run_scenarios(users)
    finally:
        server.terminate()
        server.wait(timeout=5)
    print(f"\nAll {PASSED} checks passed")
    return 0


def run_scenarios(users):
    alice, bob = Peer(PORT), Peer(PORT)

    print("Protocol & validation")
    alice.raw("hello")
    check(alice.expect("ERROR", "format") is not None, "malformed datagram -> ERROR")
    alice.raw("1|MOVE|SIDEWAYS")
    check(alice.expect("ERROR", "Direction") is not None, "invalid MOVE direction -> ERROR")
    alice.raw("abc|STATUS")
    check(alice.expect("ERROR", "id") is not None, "non numeric MSGID -> ERROR")
    check(alice.request("STATUS", reply="ERROR", contains="Not logged in") is not None,
          "command before LOGIN -> ERROR Not logged in")
    check(alice.request("REGISTER", "a", "pw", reply="ERROR", contains="Username") is not None,
          "REGISTER with invalid username -> ERROR")
    check(alice.request("FLY", reply="ERROR", contains="Unknown command") is not None,
          "unknown command -> ERROR")

    print("Reliability")
    msg_id = alice.send("STATUS")
    alice.pump(0.5)
    check(msg_id in alice.acks, "server ACKs every message (ACK|MSGID)")
    alice.inbox.clear()
    dup = alice.send("REGISTER", "alice", "secret1")
    alice.send("REGISTER", "alice", "secret1", msg_id=dup)
    alice.pump(1.0)
    oks = [m for m in alice.inbox if m[1] == "OK"]
    errors = [m for m in alice.inbox if m[1] == "ERROR"]
    check(len(oks) == 1 and not errors, "duplicate MSGID executed only once")

    lazy = Peer(PORT, auto_ack=False)
    lazy.send("STATUS")
    first = lazy.expect("ERROR", timeout=2)
    lazy.inbox.clear()
    lazy.pump(3.6)
    again = [m for m in lazy.inbox if m[0] == first[0]]
    check(len(again) >= 1, f"un-ACKed message {first[0]} retransmitted after ~3 s")

    print("Authentication")
    check(alice.request("REGISTER", "alice", "secret1", reply="ERROR", contains="taken") is not None,
          "REGISTER existing user -> ERROR")
    check(bob.request("REGISTER", "bob", "hunter22") is not None, "REGISTER bob -> OK")
    check(bob.request("LOGIN", "bob", "wrong", reply="ERROR", contains="Invalid") is not None,
          "LOGIN wrong password -> ERROR")
    with open(users) as f:
        content = f.read()
    check("secret1" not in content and "hunter22" not in content, "passwords are not stored in clear text")

    ok = alice.request("LOGIN", "alice", "secret1", "WARRIOR", contains="Welcome")
    check(ok is not None, "LOGIN alice (WARRIOR) -> OK")
    alice_id = int(re.search(r"player (\d+)", ok[2][0]).group(1))
    check(alice.expect("STATE") is not None, "LOGIN sends STATE")
    ok = bob.request("LOGIN", "bob", "hunter22", "ARCHER", contains="Welcome")
    check(ok is not None, "LOGIN bob (ARCHER) -> OK")
    bob_id = int(re.search(r"player (\d+)", ok[2][0]).group(1))
    check(bob.request("LOGIN", "bob", "hunter22", reply="ERROR", contains="Already") is not None,
          "second LOGIN on same session -> ERROR")

    print("Game")
    hp, x, y, score = alice.state()
    check((hp, score) == (100, 0) and 0 <= x < 20 and 0 <= y < 20, f"STATE|HP|X|Y|SCORE = {hp},{x},{y},{score}")
    look = alice.request("LOOK", reply="GAME_UPDATE")
    check(look is not None and look[2][0].count(";") == 1, "LOOK -> GAME_UPDATE with 2 players")
    periodic = alice.expect("GAME_UPDATE", timeout=3)
    check(periodic is not None and periodic[0] == 0, "periodic GAME_UPDATE broadcast (MSGID 0, no ACK)")

    # bob walks towards alice until she is in archer range (3)
    for _ in range(60):
        _, bx, by, _ = bob.state()
        _, ax, ay, _ = alice.state()
        if max(abs(ax - bx), abs(ay - by)) <= 3:
            break
        direction = "RIGHT" if ax > bx else "LEFT" if ax < bx else "DOWN" if ay > by else "UP"
        bob.send("MOVE", direction)
        bob.pump(0.05)
    check(max(abs(ax - bx), abs(ay - by)) <= 3, "MOVE brings bob within range")
    check(alice.request("ATTACK", str(alice_id), reply="ERROR", contains="yourself") is not None,
          "ATTACK yourself -> ERROR")

    for hit in range(4):
        check(bob.request("ATTACK", str(alice_id), contains="Hit") is not None, f"hit {hit + 1}: -20 HP")
    alice.inbox.clear()
    bob.request("ATTACK", str(alice_id), contains="eliminated")
    check(alice.expect("PLAYER_KILLED", str(alice_id)) is not None, "PLAYER_KILLED broadcast to the victim")
    check(bob.expect("PLAYER_KILLED", str(alice_id)) is not None, "PLAYER_KILLED broadcast to the killer")
    check(bob.state()[3] == 1, "killer score incremented")
    check(alice.state()[0] == 0, "victim HP = 0")
    check(alice.request("MOVE", "UP", reply="ERROR", contains="dead") is not None, "dead player cannot move")
    check(bob.request("ATTACK", str(alice_id), reply="ERROR", contains="already dead") is not None,
          "cannot attack a dead player")

    t0 = time.time()
    respawn = alice.expect("OK", "respawned", timeout=12)
    elapsed = time.time() - t0
    check(respawn is not None, f"automatic respawn (after ~{elapsed + 0.5:.0f} s)")
    check(alice.state()[0] == 100, "respawned with 100 HP")

    print("Sessions")
    carol = Peer(PORT)
    carol.request("REGISTER", "carol", "pass1234")
    carol.request("LOGIN", "carol", "pass1234", contains="Welcome")
    check(alice.request("LOGOUT", contains="Goodbye") is not None, "LOGOUT -> OK")
    check(alice.request("STATUS", reply="ERROR", contains="Not logged in") is not None, "session closed after LOGOUT")
    for _ in range(4):  # keep bob active while carol idles
        bob.state()
        time.sleep(4)
    check(carol.expect("ERROR", "timed out", timeout=2) is not None, "idle session timed out")
    check(bob.state() is not None, "active session kept alive")

    print("Brute-force protection")
    eve = Peer(PORT)
    for _ in range(5):
        eve.request("LOGIN", "carol", "badpass", reply="ERROR")
    check(eve.request("LOGIN", "carol", "pass1234", reply="ERROR", contains="Too many") is not None,
          "LOGIN blocked after 5 failures per IP")


if __name__ == "__main__":
    sys.exit(main())
