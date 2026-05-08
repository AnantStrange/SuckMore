# SuckMore

DWM-inspired but in C++. Embraces complexity and modularity instead of suckless philosophy.

## Philosophy

| DWM | SuckMore |
|-----|----------|
| Suckless | Complex |
| C | C++ |
| Monolithic | Modular |
| Config in header | Runtime config via socket |
| Patches | Plugins (layouts as polymorphic classes) |

## Architecture

Window manager core provides event handling and workspace management. Layouts are independent plugins following a pure virtual API.

WindowManager (core)<br>
│ <br>
├── Workspace x5 <br>
│   └── Tag x10 <br>
│       └── Layout (polymorphic) <br>
│ <br>
├── Socket interface (dwmstatus/cmd util) <br>
│ <br>
└── Circular double linked list <br>

Workspaces: 0 in center, 4 on edges (all 1 swipe away).<br>
x | 1 | x <br>
4 | 0 | 2 <br>
x | 3 | x <br>

## Layout API

Implement `Layout` abstract class:

- `mapWindow()` - add window to layout
- `arrange()` - position windows
- `delWindow()` - remove window
- `setFocus()` / `remFocus()` - focus handling
- MORE CHANGES TO COME !!

Layouts control their own arrangement logic. WM calls current layout's methods.

## Built-in Layouts

**MStack** (master-slave stack):
- Master area + stack area
- Adjustable master count and ratio
- Push windows up/down in stack
- Per-window height adjustment

**Planned:**
- Centered master
- Centered master floating
- Monocle
- Floating
- (Maybe) Fibonacci, Dwindle

## Data Structure

Circular double linked list enables layouts to emulate any data structure without implementing their own:

- Stack (LIFO)
- Queue (FIFO)
- List
- Circular buffer

## Communication

| Interface | Purpose |
|-----------|---------|
| Socket (cmd) | Control WM, change layouts, manage windows |
| Socket (dwmstatus) | Click handling instead of signals |
| File/socket (status) | Output workspace/tag info for external bars |

No built-in bar. `dmenu` stays independent.

## Building
```bash
cmake -S . -B build -G Ninja
cd build 
ninja
```

## Dependencies

- XCB
- XCB-RANDR
- C++20

## Current State

WIP. Layout API working. MStack implemented + WIP. Circular DLL working. Socket interface planned
FUTUREEEE.

## FAQs

<details>
<summary>Why C++? This goes against suckless philosophy!</summary>

C++ is my daily driver. I want to improve at it. Writing something this complex in C would be pain in the ass for no reason.

Suckless philosophy is great. But I'm not running a PC from the 80s, and I'm not that good of a coder to match DWM's elegance. DWM is 2000 lines of perfect C. Mine would be 2000 lines of garbage. At least in C++ I can organize the mess into classes and pretend it's intentional.
</details>

<details>
<summary>Too fucking complicated mess!</summary>

Did my best. Happy to receive suggestions.

Look at the feature set: 5 workspaces, 10 tags per workspace, per-tag layouts, layout plugin API, socket communication, circular double linked list. Try doing that in 2000 lines of C without crying. I chose complexity. I own it. Send patches if you disagree.
</details>

<details>
<summary>Why not just patch DWM?</summary>

I have and do maintain my customized version of it. Check :<br>
[dwm](https://github.com/AnantStrange/dwm.git)<br>
[StrangeDE](https://github.com/AnantStrange/StrangeDE.git)
<br>

Patching dwm taught me a lot of C, git, patches. At some point it was time to use what i learnt to
create something of my own.
</details>

<details>
<summary>Workspaces AND tags? Why both?</summary>

Workspaces = physical desktops (0 center, 4 around, one swipe away). Tags = logical grouping within each workspace.

Each workspace has its own set of 10 tags. Each tag has its own layout. You can have Firefox on workspace 0 tag 1 with master-stack layout, and terminal on workspace 2 tag 3 with monocle layout. They don't interfere.
</details>





