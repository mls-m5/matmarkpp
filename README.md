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
- [ ] Standard markdown images
- [x] Tables
- [x] Check lists
   - [ ] Nested check lists
- [x] `inline code`
- [x] code blocks
- [ ] **Bold**
- [ ] *Italic*
- [ ] ~~Strikethrough~~
- [ ] #tags


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

### Code blocks

Test with `inline code` and then a code block.

```c++
int main() {
   std::cout << "hello there\n" << std::endl;
}
```

Or raw html
```html
   <div><b>hello</b></div>
```
