# TermNote

**TermNote** is a simple, fast, and minimal terminal-based note-taking application written in C using `ncurses`. It lets you create, view, and manage notes directly from your terminal in a clean and keyboard-driven interface on unix-like systems.

<img width="1776" height="1084" alt="2025-07-22_14-59_ternnote" src="https://github.com/user-attachments/assets/d1b1e847-085e-4714-a853-e345fc082409" />

---

## ✨ Features

- Lightweight and fast
- Fully keyboard-driven interface
- Persistent notes stored under XDG standard locations
- Highlight selected note
- Split-pane view for note list, content, and options
- Simple plain-text storage format
- Built using `ncurses`

---

## 📦 Installation

### Dependencies

- `ncurses`
- A C compiler (`gcc`, `clang`, etc.)
- `make`

### Build and Run

```bash
git clone https://github.com/highpn/termnote.git
cd termnote
make
./termnote
