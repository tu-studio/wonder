# TODO

- [ ] Add runpaths to the final executables
- [ ] Uncomment the saving of old clients
- [ ] I think we should use user services at boot, therefore we need to make the studio user a lingering user with `loginctl enable-linger studio`. We can then use `loginctl user-status studio` to check if the user is a linger user.
