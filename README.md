HTML from QML generator (Prove of Concept)

Example of input file:

```qml
import HTML 1.0

Page {
    title: "HTML Example"

    Heading {
        level: 1
        text: "This is a heading"
    }

    Paragraph {
        text: "This is a paragraph."
    }

    Button {
        text: "Click me!"
    }
}
```

Example of output:

```html
<html>
<head>
<title>HTML Example</title>
</head>
<body>
<h1>This is a heading</h1>
<p>This is a paragraph.</p>
<button>Click me!</button>
</body>
</html>
```
