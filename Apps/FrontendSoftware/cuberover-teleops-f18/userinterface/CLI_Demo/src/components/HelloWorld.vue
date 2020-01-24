<template>
  <div class="hello">
    <h1>CubeRover CLI Demo</h1>
    <textarea 
      name="cli-input" 
      id="cli-input" 
      v-model="message"
      @input="updateValue()"
      ref="input"
      ></textarea>
    <p class = "cliDisplay">
      <span class="cliDisplay__userInput">{{ message }}</span>
      <span class="cliDisplay__autoFill">{{ autofill }}</span>
    </p>
  </div>
</template>

<script>
export default {
  name: "HelloWorld",
  data: function() {
    return {
      message: "",
      autofill: "",
      commands: [
        // commands autofill in order of this list
        ["left", ["Angle", "Speed", "Acceleration", "Delay"]],
        ["lockwheels", ["end", "side"]],
        ["loadconfig", ["name", "version"]]
      ]
    };
  },
  methods: {
    updateValue: function() {
      const msgLength = this.message.length;
      for (var command of this.commands) {
        // checks to see if entered text matches command
        if (
          command[0].substring(0, msgLength) === this.message &&
          msgLength > 0
        ) {
          // constructs autofill for first command that matches
          const cmdEnd = command[0].substring(msgLength, command[0].length);
          let cmdVars = "[ " + command[1].toString() + " ]";
          cmdVars = cmdVars.replace(/,/gi, ", ");
          this.autofill = cmdEnd + " " + cmdVars;
          return;
        } else {
          // when command no longer matches autofill is emptied
          this.autofill = "";
        }
      }
      if (this.message === "foo") {
        this.message = "bar";
      }
    }
  }
};
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped lang="scss">
#cli-input {
  width: 100%;
  resize: none;

  // &:focus {
  //   height: 0;
  //   margin: 0;
  //   padding: 0;
  //   border: none;
  // }
}

.cliDisplay {
  background-color: #d8d8dd;
  padding: 1rem;
  border-radius: 4px;
  text-align: left;

  &__userInput {
    color: #2d2d2d;
    font-weight: bold;
  }

  &__autoFill {
    color: #878787;
  }
}
</style>
