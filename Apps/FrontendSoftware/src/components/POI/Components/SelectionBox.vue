<script>
export default {
  name: "SelectionBox",
  props: {
    startCoord: Array,
    endCoord: Array,
    isMouseDown: Boolean,
    isDrag: Boolean,
  },

  methods: {
    // On mouse down in image port, draw rectangle selector and get start coordinates
    onMouseDown(event) {
      console.log("mouseDown");
      // Ignore right click
      if (event.button === 2) {
        return;
      }

      // reset end coord if already exists
      if (this.endCoord.length > 0) {
        this.endCoord = [];
      }

      this.startCoord = [event.offsetX, event.offsetY];
      this.isMouseDown = true;

      // false until proven truthy (aka don't know if drag or click until onMouseMove called or not called)
      this.isDrag = false;
    },

    onMouseMove(event) {
      if (this.isMouseDown) {
        this.isDrag = true;
        this.endCoord = [event.offsetX, event.offsetY];

        // Ensures selection box constrained to image
        if (
          !(this.endCoord[0] <= this.poiLayer.width) ||
          !(this.endCoord[0] >= 0) ||
          !(this.endCoord[1] <= this.poiLayer.height) ||
          !(this.endCoord[1] <= this.poiLayer.width) ||
          !(this.endCoord[1] >= 0)
        ) {
          this.isMouseDown = false;

          // if cursor went past right boundary of image container
          if (this.endCoord[0] > this.poiLayer.width) {
            this.endCoord[0] = this.poiLayer.width;
          }
          // if cursor went past right boundary of image container
          else if (this.endCoord[0] < 0) {
            this.endCoord[0] = 0;
          }
          // if cursor went past top boundary of image container
          if (this.endCoord[1] > this.poiLayer.height) {
            this.endCoord[1] = this.poiLayer.height;
          }
          // if cursor went past bottom boundary of image container
          else if (this.endCoord[1] < 0) {
            this.endCoord[1] = 0;
          }
        }

        this.setUpSelection();
      }
    },

    setUpSelection() {
      let topLeft = this.startCoord; // x1, y1
      let topRight = [this.endCoord[0], this.startCoord[1]]; //x2, y1
      let bottomRight = this.endCoord; // x2, y2
      let bottomLeft = [this.startCoord[0], this.endCoord[1]]; //x1, y2

      // Sets POI Layer's dimensions, while also clearing canvas of any drawings currently on it
      this.setPOILayerDimensions();

      // Begin: drawing POI selector box on canvas
      this.poiCanvasContext.beginPath();

      // Style of selection box
      this.poiCanvasContext.strokeStyle = "#D5D5D5";
      this.poiCanvasContext.lineWidth = 2;

      // Start drawing selector box ("rect")
      this.poiCanvasContext.moveTo(topLeft[0], topLeft[1]);

      this.poiCanvasContext.lineTo(topRight[0], topRight[1]); // Top side of rect
      this.poiCanvasContext.lineTo(bottomRight[0], bottomRight[1]); // Right side of rect
      this.poiCanvasContext.lineTo(bottomLeft[0], bottomLeft[1]); // Bottom side of rect
      this.poiCanvasContext.lineTo(topLeft[0], topLeft[1]); // Left side of rect

      this.poiCanvasContext.stroke();
      this.poiCanvasContext.closePath();
    },

    onMouseUp() {
      this.isMouseDown = false;

      // Checks if mouse down was just a click or if mouse moved at all (i.e. selection box was formed)
      if (this.endCoord.length == 0) {
        this.startCoord = [];

        //clears canvas of lines/boxes
        this.setPOILayerDimensions();
      }

      // coordinates
      console.log(
        "Start coordinates: ",
        this.startCoord,
        "\nEnd Coordinates: ",
        this.endCoord
      );
    },

    setPOILayerDimensions() {
      this.poiLayer.height = this.canvas.offsetHeight;
      this.poiLayer.width = this.canvas.offsetWidth;
    },
  },
};
