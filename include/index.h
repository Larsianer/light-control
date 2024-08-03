const char* HTML_CONTENT = R"""(
<!DOCTYPE html>
<html>
    <head>
        <title>
            Light control for my room
        </title>
        
        <style>
body {
    background-color: #303030;
}

label {
    font: 1rem sans-serif;
    color: white;
}

#update {
    margin: 5px;
}

.container {
    padding: 10px;
    margin: 10px;
    border-width: 2px;
    border-radius: 4px;
    border-color: white;
    border-style: solid;
    box-sizing: border-box;
    width: min-content;
}
        </style>
        <script>
            function previewColor() {
                let hue = document.getElementById("hue").valueAsNumber / 255 * 360;
                let sat = document.getElementById("sat").valueAsNumber / 255;
                let val = document.getElementById("val").valueAsNumber / 255; 
                let light = val * (1 - (sat / 2));
                if (light == 0 || light == 1) {
                    sat = 0;
                } else {
                    sat = (val - light) / Math.min(light, 1 - light);
                }
                document.getElementById("color-preview").style.backgroundColor = `hsl(${hue}, ${sat * 100}%, ${light * 100}%)`;
            }
            
        </script>
        <meta name="viewport" content="width=device-width,initial-scale=1" />
    </head>
    <!-- execute previewColor once on page load to directly show the correct color --!>
    <body onload="previewColor()">
        <form method="post">
            <div class="container">
                <input type="checkbox" name="enable" id="enable"/>
                <label for="enable">Enable LEDs</label>
                <input type="submit" id="update" value="Update LEDs">
            </div>
            <div class="container" id="color-preview">
            </div>
            <div class="container" id="color-picker">
                <input type="range" min=0 max=255 name="hue" id="hue" class="color-picker-range" oninput="previewColor()"/>
                <label for="hue">Hue</label>
                <input type="range" min=0 max=255 name="sat" id="sat" class="color-picker-range" oninput="previewColor()"/>
                <label for="sat">Saturation</label>
                <input type="range" min=0 max=255 name="val" id="val" class="color-picker-range" oninput="previewColor()"/>
                <label for="val">Value</label>
            </div>
        </form>
    </body>
</html>
)""";
