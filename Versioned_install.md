# Info on symlink structure for versioned install flag

## Binaries

```path
/usr/local/bin/
├── cwonder -> /usr/local/bin/cwonder-<version>
├── cwonder-<version>
├── twonder -> /usr/local/bin/twonder-<version>
└── twonder-<version>
```

## Scripts and dtd

```path
/usr/local/share/
├── wonder -> /usr/local/share/wonder-<version>
└── wonder-<version>
```

The systemd service files are not versioned.
