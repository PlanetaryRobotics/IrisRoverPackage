## Current List of High-Level Outstanding Edits for the CLI Component:
### Last edit: 2/7/2019, Colombo

* STICKY FOOTER - Make the CommandField stick to the bottom.

* Make CLI, particularly Commands, behave nicely when placed in a confined space
..* Make CommandField dock to the bottom of confining div
⋅⋅* Make Commands not show extra inline data (eg, timestamp) if there's not enough space

* Make Log autoscroll to bottom when new commands are issued

* Load CommandOptions from CSV file

* Type checking and input validation on commands as arguments are entered (involves extra type element to CommandOption's param object)

* Make waiting or other select commands stick to the top (.sticky class)
..* NB. Proper behaviour for this has proven a bit dicey

* Add search (filter) functionality to Log

* Make Command functions get called within context of Command component so that they can   directly return data

* Smooth out transitions for Command info drop-downs

* Pop-under help dialog for what the inputs mean (optional)

* Fix bug where you can't grab scrollbar in Log (only wheel scrolling works)

* Implement better autocomplete (and autocorrect).
..* Must also factor in useCount of commands, and hold that at a higher priority.
..* Compute string distance vector between inputText and all command names.
..* Find some aesthetic way inline way of suggesting corrections ("TunrLe..." -> "TurnLe...")

* Refactor to use "rem" rather than "em" for all dimensions and add global css reset
