 # Shortcut Keys for Highlevel Interface Components

 This document outlines the standard for how shortcut keys should be captured within highlevel interface components. **For example**, if you want the image to be maximized whenever `ctrl+m` is clicked anywhere in the ImageViewer, you'd follow the following procedure.

 ## Specifying Keys

 Specifying the key to capture is pretty straight forward, just use the event definitions provided in the [Vue Event Documentation](https://vuejs.org/v2/guide/events.html).
 
 In the case of capturing `ctrl+m` you just need to do `@keyup.ctrl.m` or `@keyup.ctrl.77` which uses the keyCode, which you can find using this [tool](https://keycode.info/). Though, note that keyCodes may be deprecated in Electron in the future; so, try to avoid using them.
 
 You may also want to consider using the `.exact` modifier, as in `@keyup.ctrl.m.exact` to avoid also capturing longer strings like `ctrl+shift+m`.

 ## Detecting the Keypress Everywhere

 If you try the above on a highlevel component, you'll notice it doesn't work outside of input fields. This is because nothing has the keyboard's focus.
 
 A quick but effective hack around this is to make the entire container able to capture the keyboard's focus by adding the `tabindex="0"` attribute to the highlevel component container.

 ## General Example:
 
```
  <div 
    id="highlevelComponentContainer"
    tabindex="0"
    @keyup.ctrl.m="maximizeImage"
  >
    <!-- CONTENTS OF YOUR HIGHLEVEL COMPONENT -->
  </div>
```

----

## Naming Convention for Key Press Events on the Global `$eventHub`

You may want to detect keypress events on a highlevel component container and then broadcast an event that can be read by all of its children. If you use the global `$eventHub` for this, name your event based on the high-level category of the components being targeted and the keypressed.

For example, if you want to trigger an event when the `left` arrow is pressed in the IV, name the event: `IV-keypress-left`.

The following list enumerates the keypress event options:
- **Clicked anywhere**: `GLOBAL-keypress-XXX`
- **Clicked inside CommandLine**: `CLI-keypress-XXX`
- **Clicked inside ImageViewer**: `IV-keypress-XXX`
- **Clicked inside Telemetry**: `TELEM-keypress-XXX`
- **Clicked inside ErrorAnalysis**: `EA-keypress-XXX`
- **Clicked inside Map**: `MAP-keypress-XXX`
- **Clicked on Infrastructure (eg Combined Screens)**: `INFRA-keypress-XXX`

----

 ## Preventing Capture on Certain Regions (like inputs)

### Problem Area
 The above works great unless the key pattern you want to capture is something simple which has a normal use, like the `arrow` keys in scrollable regions and text boxes or just any letter in a text field.

 For example, if you want to map the `e` key to some function in Error Analysis, you wouldn't also want that function getting fired everytime the user presses the `e` key in a textbox.

### Solution
 To keep this event from bubbling up, you can specifically stop the event that's getting captured. **For example**, if `@keydown.e` is getting captured, then you'd want to add `@keydown.stop` to the input field where you want the user to be able to type without triggering events.
 
 Alternatively, it is **recommended** that you just stop all keypress events from bubbling out of the input field as follows:
 ```
 <input 
    type="text"
    @keyup.stop
    @keydown.stop
 />
 ```

### Overriding the Solution
If, however, you want to then add some special shortcut key command which works even while a user is typing in any input field inside the component, you can use the `.capture` modifier.

#### Using the Capture Path to Override
For example, if you want `ctrl+left` to change something even if the user is also using the left arrow to move where they're typing in a textbox, you can capture by doing `@keyup.ctrl.left.exact.capture="somethingVeryImportant"`.

**Note:** It would be bad design to do an override like this for a key or sequence a user might reasonably want to use in a textbox without having secondary effects, example: any `alphanumeric` key, plain `arrow` keys, `shift+arrow`, etc.

#### Prevent Side Effects
Remember: events normally bubble up (get called on) the DOM element tree first where you clicked, then on its parent, to its parent, to its parent, etc. but, when you add capturing, you add a second process where its called from the root node down through the children up to where you clicked. So, if you add capturing and don't want an edge case where the event also gets called on elements between the container and the target node, you'll need to call `.capture.stop`.

#### Final example
```
  <div 
    id="highlevelComponentContainer"
    tabindex="0"
    @keyup.ctrl.m="maximizeImage"
    @keyup.ctrl.left.exact.capture.stop="somethingVeryImportant"
  >
    <!-- CONTENTS OF YOUR HIGHLEVEL COMPONENT -->
  </div>
```
