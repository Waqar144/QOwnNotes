# Installa su Raspberry Pi OS

## Raspberry Pi OS 10

Eseguire i seguenti comandi della shell per considerare attendibile il repository.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_10/Release.key -O - | sudo apt-key add -
```

Eseguire i seguenti comandi della shell per aggiungere il repository e installare QOwnNotes da lì.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_10/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

[Download diretto](https://build.opensuse.org/package/binaries/home:pbek:QOwnNotes/desktop/Raspbian_10)

## Raspbian 9.0

Eseguire i seguenti comandi della shell per considerare attendibile il repository.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_9.0/Release.key
-O - | sudo apt-key add -
```

Eseguire i seguenti comandi della shell per aggiungere il repository e installare QOwnNotes da lì.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_9.0/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

[Download diretto](https://build.opensuse.org/package/binaries/home:pbek:QOwnNotes/desktop/Raspbian_9.0)
