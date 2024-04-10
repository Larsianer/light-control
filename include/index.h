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
}

#update {
    margin: 5px;
}

.container {
    padding: 10px;
    margin: 10px;
    border-width: 1px;
    border-color: white;
    border-style: solid;
    box-sizing: border-box;
    width: min-content;
}
        </style>
        <meta name="viewport" content="width=device-width,initial-scale=1" />
    </head>
    <body>
        <form method="post">
            <div class="container">
                <input type="checkbox" name="enable" id="enable"/>
                <label for="enable">Enable LEDs</label>
                <input type="submit" id="update" value="Update LEDs">
            </div>
            <div class="container" id="color-picker">
                <input type="range" min=0 max=255 name="hue" id="hue" class="color-picker-range"/>
                <label for="hue">Hue</label>
                <input type="range" min=0 max=255 name="sat" id="sat" class="color-picker-range"/>
                <label for="sat">Saturation</label>
                <input type="range" min=0 max=255 name="val" id="val" class="color-picker-range"/>
                <label for="val">Value</label>
            </div>
        </form>
    </body>
</html>
)""";
