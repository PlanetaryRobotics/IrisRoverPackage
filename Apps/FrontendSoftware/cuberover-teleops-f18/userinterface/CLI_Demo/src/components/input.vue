<template>
  <div class="cli-input__container">
    <input 
      name="cli-input" 
      id="cli-input" 
      v-model="userInput"
      v-on:keydown.tab.prevent="tabPressed"
      v-on:keydown.enter.prevent="enterPressed"
      v-on:keydown.esc.prevent="escapePressed"
      @input="keyPressed"
      ref="input"
      spellcheck="false"
      type="text"
      />
    <p class = "cliDisplay" v-if="increment === 0">
      <span class="cliDisplay__userInput">>  {{ userInput }}</span>
      <span class="cliDisplay__autoFill">{{ autofill }}</span>
    </p>
    <p class = "cliDisplay" v-if="increment > 0">
      <span class="cliDisplay__userInput">>  {{ command.Name }} [ {{ preFocused }}</span>
      <span class="cliDisplay__focused">{{ focused }}</span>
      <span class="cliDisplay__userInput"> {{postFocused}} ]</span>
    </p>
  </div>
</template>

<script>
export default {
  name: "HelloWorld",
  data: function() {
    return {
      userInput: "",
      autofill: "",
      focused: "",
      preFocused: "",
      postFocused: "",
      increment: 0,
      command: {
        Name: ""
      },
      commands: this.$store.state.commands,
      variables: this.$store.state.variables
    };
  },
  methods: {
    tabPressed: function() {
      if (this.increment == 0) {
        for (let key in this.commands) {
          let cmdName = this.commands[key].name;
          if (cmdName.substring(0, this.userInput.length) === this.userInput) {
            this.fillCommand(cmdName);
            break;
          }
        }
      } else {
        this.fillVar();
        this.increment++;
      }
      this.autoFill();
    },

    enterPressed: function() {
      if (this.command.Name != "") {
        this.tabPressed();
        for (let variable of this.commands[this.command.Name].variables) {
          if (this.command[variable] == undefined) {
            this.command[variable] = this.variables[variable].default;
          }
        }
        this.command.status = "waiting";
        let time = new Date();
        this.command.Time = time.getTime();
        this.$store.state.history.push(this.command);
        this.command = { name: "" };
        this.userInput = "";
        this.increment = 0;
        this.autofill = "";
      }
    },

    escapePressed: function() {
      this.command = { name: "" };
      this.userInput = "";
      this.increment = 0;
      this.autofill = "";
    },

    keyPressed: function() {
      this.autofill = "";
      if (this.increment === 0) {
        for (let key in this.commands) {
          let cmdName = this.commands[key].name;
          if (cmdName === this.userInput) {
            this.fillCommand(cmdName);
          }
        }
      }
      this.autoFill();
    },

    autoFill: function() {
      let cmdFill = "";
      let varFill = "";
      // autofill if we are typing command
      if (this.increment === 0 && this.userInput != "") {
        for (let key in this.commands) {
          let cmdName = this.commands[key].name;
          if (cmdName.substring(0, this.userInput.length) === this.userInput) {
            cmdFill = cmdName.substring(this.userInput.length, cmdName.length);
            varFill = "[ " + this.commands[key].variables.toString() + " ]";
            varFill.replace(/,/gi, ", ");
            break;
          }
        }
        this.autofill = cmdFill + " " + varFill;
      } else {
        // autofill if we are typing variable
        var index = 0;
        this.preFocused = "";
        this.focused = "";
        this.postFocused = "";
        for (let variable of this.commands[this.command.Name].variables) {
          index++;
          if (
            this.increment > this.commands[this.command.Name].variables.length
          ) {
            this.increment = 1;
          }
          if (index < this.increment) {
            this.preFocused += this.autoFillVar(variable) + ", ";
          } else if (index == this.increment) {
            this.focused = this.autoFillVar(variable, true);
          } else {
            this.postFocused += this.autoFillVar(variable) + ", ";
          }
        }
        // Removes trailing comma and space.
        this.postFocused = this.postFocused.slice(0, -2);
        index = 0;
      }
    },

    autoFillVar: function(variable, focused = "false") {
      if (focused == true && this.userInput != "") {
        return this.userInput + this.variables[variable].units;
      } else if (this.command[variable] != undefined) {
        return this.command[variable] + this.variables[variable].units;
      } else {
        return variable;
      }
    },

    fillCommand: function(cmdName) {
      this.command.Name = cmdName;
      this.userInput = "";
      this.increment++;
    },

    fillVar: function() {
      let variable = this.commands[this.command.Name].variables[
        this.increment - 1
      ];
      if (this.userInput == "" && this.command[variable] == undefined) {
        this.command[variable] = this.variables[variable].default;
      } else {
        if (this.userInput != "") {
          this.command[variable] = this.userInput;
        }
        this.userInput = "";
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
  position: absolute;
  padding: 0;
  margin: 0;
  top: 0;
  left: 0;
  height: 100%;
  background-color: transparent;
  color: transparent;
  border: none;
  border-radius: 0.5rem;
  transition: 0.15s ease;
  border-bottom: 0.5rem solid #d8d8dd;

  &:focus {
    border: none;
    outline: none;
    border-bottom: 0.4rem solid #67cc77;
  }
}
.cli-input__container {
  position: relative;
  margin-bottom: 3rem;
}
.cliDisplay {
  background-color: #d8d8dd;
  padding: 1.5rem;
  border-radius: 0.5rem;
  text-align: left;

  &__userInput {
    color: #2d2d2d;
    font-weight: bold;
  }

  &__autoFill {
    color: #878787;
  }

  &__focused {
    background-color: #2d2d2d;
    border-radius: 0.5rem;
    color: #f1f1f1;
    padding: 0.3rem 0.6rem;
  }
}
</style>
