# Running Fil-C OpenSSH as Your System sshd

This document explains how to run the Fil-C build of OpenSSH at
`/opt/fil/sbin/sshd` instead of what comes on your system. Most of the work
to make this possible should have already happened if you ran `./setup.sh`.
This document explains what that script did as well as some final steps you
will have to take manually.

Fil-C is open source software provided AS IS, without warranty of any kind.
Nothing in this document is a guarantee of security or fitness for any
purpose. You should test the Fil-C OpenSSH server in your own environment
before relying on it, especially before pointing systemd at it on a machine
you care about. The procedure described below for switching systemd over to
a different sshd binary can lock you out of a remote machine if something
goes wrong, so read the section on testing and rollback carefully before you
start, and do not attempt it without console access or a second working way
in.

## Why use the Fil-C OpenSSH server

The Fil-C build of OpenSSH eliminates a broad class of memory safety bugs,
including things like the regreSSHion vulnerability (CVE-2024-6387). All
C files in OpenSSH and its dependencies (including OpenSSL, zlib, glibc,
pam, and other libraries) have been compiled with Fil-C. Fil-C catches all
memory safety issues at [runtime as Fil-C panics](https://fil-c.org/how).
The Fil-C port retains all of [OpenSSH's seccomp based sandbox
logic](https://fil-c.org/seccomp) and all of [OpenSSL's constant time crypto
hardening](https://fil-c.org/constant_time_crypto). Fil-C's performance
and memory usage overhead doesn't seem to impact OpenSSH under normal use
(even when doing things like large file transfers or X11 forwarding). The
included OpenSSH build supports common Linux sshd features like:

- pam
- seccomp
- systemd socket activation (but without a dependency on systemd)
- gssapi for authentication and kex

`/opt/fil/sbin/sshd` is based on OpenSSH 10.3p1 with small changes for Fil-C
compatibility plus Debian patches to support things like gssapi and systemd.

Using `/opt/fil/sbin/sshd` as a replacement has been tested on Ubuntu,
Rocky, Fedora, Debian, and Omarchy.

## What `setup.sh` already did for you

When you ran `./setup.sh`, the installer did a fair amount of work to make
`/opt/fil/sbin/sshd` ready to run as a system service. It is worth
understanding what was actually changed on your system so you know what is
already taken care of and what is not.

If `/etc/ssh` did not exist, the installer created it. If the standard
configuration files (`ssh_config`, `sshd_config`, and `moduli`) were
missing, the installer copied them from
`/opt/fil/share/examples/ssh/`. Existing config files were left alone, so
if you already had a system OpenSSH installation the installer did not
touch your local customizations.

If any of the standard host keys (`ssh_host_rsa_key`,
`ssh_host_ecdsa_key`, and `ssh_host_ed25519_key`, along with their `.pub`
counterparts) were missing, the installer ran `/opt/fil/bin/ssh-keygen -A`
to generate them. Existing host keys were preserved. The installer also
checked the permissions on the host private keys. If any key was mode
`0640` and owned by the `ssh_keys` group, which is the old Red Hat
convention, the installer changed it to mode `0600`, which is the
convention that the Fil-C `/opt/fil/sbin/sshd` expects. Keys with
stranger permission patterns were left alone and reported as warnings,
since the installer cannot know whether those permissions are intentional.

The `sshd` privilege separation user and group were created if they were
missing. The user is created with `/opt/fil/var/lib/sshd` as its home
directory and `/bin/false` as its shell, which is the standard non-login
configuration. The installer also creates `/opt/fil/var/lib/sshd` itself
if it does not exist, since sshd's privilege separation can fail at
runtime if that directory is missing. If the user or group already
existed, the installer did not touch it.

Unless you used the `-u --fill-setup` options, `./setup.sh` would have
prompted you before making any changes to your system's SSH configuration.
If you did not see any prompt, this means that the script determined that
your existing setup was already sufficient.

Finally, the installer attempted to apply the right SELinux label to
`/opt/fil/sbin/sshd`. The logic is conservative. If SELinux is not active
in the kernel, the installer does nothing. If SELinux is active but the
`chcon` tool is not installed, the installer warns you and asks you to
install the SELinux user-space tools and label the binary by hand. If
SELinux is active and the tools are present, the installer reads the
label from `/usr/sbin/sshd` and, if that label has the standard
`sshd_exec_t` type, copies the same label onto `/opt/fil/sbin/sshd`
using `chcon --reference=/usr/sbin/sshd /opt/fil/sbin/sshd`. If the label
on `/usr/sbin/sshd` has a different type, the installer refuses to guess
and prints detailed instructions for what to try by hand. This is one of
the cases where you may need to do the SELinux labeling yourself before
the binary should work under systemd.

## Switching systemd over to `/opt/fil/sbin/sshd`

Before you change anything, understand the lockout risk. If you point
systemd at a misconfigured sshd, the service may fail to start, or it may
start but refuse logins. On a remote machine without console or
out-of-band access, that situation can leave you unable to get back in.
Before starting this procedure, make sure you have a way to recover if
sshd stops accepting logins. That can be a serial or KVM console, a
hypervisor console, a separate management interface, physical access, or
at minimum a second already-authenticated SSH session that you keep open
throughout the switchover. If none of these is available, do not do this
on a production machine. Try it on a disposable VM first.

To run the Fil-C build as your actual system sshd, you need to point your
existing systemd unit at the new binary, restart the service, and confirm
that you can still log in before you log out.

The cleanest way to do this without losing your distribution's unit file
is to drop a systemd override. On most distributions the unit is called
`sshd.service` (Red Hat, Rocky, Fedora, and CentOS) or `ssh.service`
(Debian and Ubuntu). Find out which one by running `systemctl status ssh`
or `systemctl status sshd`. Before writing the override, run `systemctl
cat sshd.service` (or `ssh.service`) and read through the upstream unit.
The override below replaces the `ExecStartPre`, `ExecStart`, and
`ExecReload` directives. If your distribution's unit relies on additional
hardening directives, environment files, socket activation units, or
other settings layered on top of `ExecStart`, those will continue to
apply, but you should sanity-check that the override below is still
appropriate for the unit your distribution ships. Distributions update
their sshd units from time to time, so it is worth rechecking the
override after major distribution upgrades.

Run `systemctl edit sshd.service` (or `ssh.service`), which will open an
empty override file in your editor. Put the following into the override
and save it.

```ini
[Service]
ExecStartPre=
ExecStart=
ExecReload=
ExecStartPre=/opt/fil/sbin/sshd -t
ExecStart=/opt/fil/sbin/sshd -D $SSHD_OPTS
ExecReload=/opt/fil/sbin/sshd -t
ExecReload=/bin/kill -HUP $MAINPID
```

The empty assignments are important. They clear the values inherited from
the distribution unit before the new values are appended. Without them you
would end up with both binaries listed and systemd would refuse to start
the service.

Before restarting the service, validate the configuration with
`/opt/fil/sbin/sshd -t`. This catches typos in `sshd_config` and missing
host keys without taking the service down. It does not catch every runtime
failure: PAM stack mismatches, SELinux denials, AppArmor profiles, systemd
sandboxing directives inherited from the distribution unit, seccomp
restrictions, capability mismatches, and port conflicts will only show up
when sshd actually tries to start. A clean `-t` is a necessary check, not
a sufficient one.

Keep one root SSH session open while you do the switchover. Open a second
SSH session from somewhere else to confirm that logins still work before
you close the first session. If something goes wrong, you can usually
recover from the still-open session by running `systemctl revert
sshd.service` (or `ssh.service`) and restarting the service. Note that
revert only removes the override you added. It does not undo the host
keys, the SELinux labels, the `sshd` user, or any other changes that
`setup.sh` made earlier. If you need to fully unwind those, you will have
to do it manually.

Reload systemd and restart the service:

```bash
sudo systemctl daemon-reload
sudo systemctl restart sshd   # or 'ssh' on Debian/Ubuntu
sudo systemctl status sshd
```

If the status output shows `active (running)` and the `ExecStart=` line
points at `/opt/fil/sbin/sshd`, the systemd unit is now running the
Fil-C build of OpenSSH. Open a second terminal and test a login before
doing anything else.

If the service fails to start on a system that uses SELinux, the most
likely cause is that the new binary is missing the SELinux label that
sshd is expected to have. The installer tries to handle this
automatically, as described above, but if it printed a warning about an
unrecognized label on `/usr/sbin/sshd`, or could not find the SELinux
tools, you will need to fix this by hand before the service will work.
The simplest fix is to copy whatever label the stock sshd has:

```bash
sudo chcon --reference=/usr/sbin/sshd /opt/fil/sbin/sshd
```

That sets the label for now, but a future `restorecon` run or full
SELinux relabel will undo it, because the persistent file context
database does not know about `/opt/fil/sbin/sshd`. To make the label
stick across relabels, register it with `semanage`. Replace `sshd_exec_t`
below with whatever type your distribution actually uses, which you can
read off the output of `ls -Z /usr/sbin/sshd`.

```bash
sudo semanage fcontext -a -t sshd_exec_t '/opt/fil/sbin/sshd'
sudo restorecon -v /opt/fil/sbin/sshd
```

If `semanage` is not installed, you can install it with `dnf install
policycoreutils-python-utils` on Rocky and Fedora, or the equivalent
package on your distribution.

After the labeling change, restart the service again and check
`journalctl -u sshd` if it still misbehaves. The journal will usually
tell you which file or socket SELinux denied access to, which makes it
easier to track down any remaining policy mismatch. On distributions
that use AppArmor instead of SELinux (Debian and Ubuntu by default)
there is an analogous risk that the distribution's AppArmor profile for
sshd does not cover `/opt/fil/sbin/sshd`. If you see denials in
`dmesg` or the journal that mention `apparmor=`, you will need to
either disable the relevant profile or adjust it to cover the new path.

Finally, if you want to roll back at any point, `systemctl revert
sshd.service` deletes the override you added and returns the unit to
the distribution sshd, and a subsequent `systemctl restart sshd` brings
it back online. As noted above, revert only undoes the override itself.
The other changes that `setup.sh` made, including host keys, SELinux
labels, and the `sshd` user account, are not removed by revert and you
will need to clean those up by hand if you want a fully clean
uninstall.
