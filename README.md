# matmarkpp

Markdown in c++ (Obsidian.md flavor)

## Implemented features

- [x] Headers
- [x] "![[]]" syntax for images
  - [ ] Support for non full line
- [ ] Include other md files with "![[]]"
- [x] "[[]]" syntax links
  - [ ] Support for non full line links
- [x] Horizontal lines
- [x] Unordered lists
- [ ] Ordered lists
- [ ] Standard markdown links
- [x] Tables
- [x] Check lists
   - [ ] Nested check lists
- [ ] inline code
- [ ] code blocks


# Project structures

- `include`: contains files to be included
- `src`: source files

- `main`: files for running lib as standalone application



## Tests

- List
- List
   - Nested

[[README.md]]

![[img.png]]

### A small table
|  left column  |  right column   |
| ------------- | --------------- |
| value 1       | value 2         |
| value 3       | value 4         |
