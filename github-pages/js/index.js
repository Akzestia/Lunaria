import { QuicClient, Wayland, QuicServer } from "./texts.js";

render("Client");

function render(title) {
  const rootElement = document.getElementById("root");
  switch (title) {
    case "Akzestia":
      {
        var name = `<h1>Akzestia</h1>`;
        rootElement.innerHTML = name;

        var block1_text;

        let x = styleText(QuicClient);
        block1_text = `<pre>${x}</pre>`;
        var block1 = `<div class="container">${block1_text}</div>`;
        rootElement.innerHTML += block1;
      }
      break;
    case "Client":
      {
        var name = `<h1>Client</h1>`;
        rootElement.innerHTML = name;

        var block1_text;

        let x = styleText(QuicClient);
        block1_text = `<pre>${x}</pre>`;
        var block1 = `<div class="container">${block1_text}</div>`;
        rootElement.innerHTML += block1;
      }
      break;
    case "Server":
      {
        var name = `<h1>Server</h1>`;
        rootElement.innerHTML = name;

        var block1_text;

        let x = styleText(QuicServer);
        block1_text = `<pre>${x}</pre>`;
        var block1 = `<div class="container">${block1_text}</div>`;
        rootElement.innerHTML += block1;
      }
      break;
    case "Wayland":
      {
        var name = `<h1>Wayland</h1>`;
        rootElement.innerHTML = name;

        var block1_text;

        let x = styleText(Wayland);
        block1_text = `<pre>${x}</pre>`;
        var block1 = `<div class="container">${block1_text}</div>`;
        rootElement.innerHTML += block1;
      }
      break;
    case "X11":
      {
        var name = `<h1>X11</h1>`;
        rootElement.innerHTML = name;
      }
      break;
    case "Qt":
      {
        var name = `<h1>Qt</h1>`;
        rootElement.innerHTML = name;
      }
      break;
    case "MsQuic":
      {
        var name = `<h1>MsQuic</h1>`;
        rootElement.innerHTML = name;
      }
      break;
    case "Linux":
      {
        var name = `<h1>Linux</h1>`;
        rootElement.innerHTML = name;
      }
      break;
    case "Windows":
      {
        var name = `<h1>Windows</h1>`;
        rootElement.innerHTML = name;
      }
      break;
    case "Dev Team":
      {
        var name = `<h1>Dev Team</h1>`;
        rootElement.innerHTML = name;
      }
      break;
    case "FAQ":
      {
        var name = `<h1>FAQ</h1>`;
        rootElement.innerHTML = name;
      }
      break;
    default:
      break;
  }
}

function styleText(text) {
  text = text.replace(/</g, "&lt;");
  text = text.replace(/>/g, "&gt;");

  const keywords =
    /\b(auto|bool|break|case|char|class|const|continue|default|do|double|else|enum|extern|float|for|goto|if|inline|int|long|namespace|new|operator|private|protected|public|register|return|short|signed|sizeof|static|struct|switch|template|typedef|union|unsigned|using|virtual|void|volatile|while)\b/g;
  const types =
    /\b(int|float|double|char|long|short|signed|unsigned|bool|void)\b/g;
  const classNames = /\b([A-Z][a-zA-Z0-9]*)\b/g;
  const functionNames = /\b([a-z_][a-zA-Z0-9_]*(?=\())/g;
  const brackets = /[\(\)\[\]\{\}]/g;
  const numbers = /\b(\d+(\.\d+)?(e[+-]?\d+)?)\b/gi;
  const strings = /"(.*?)"|'(.*?)'/g;
  const comments = /(\/\/[^\n]*|\/\*[\s\S]*?\*\/)/g;

  text = text.replace(strings, '<span data-style="green">$&</span>');
  text = text.replace(comments, '<span data-style="grey">$&</span>');
  text = text.replace(numbers, '<span data-style="purple">$&</span>');
  text = text.replace(keywords, '<span data-style="red">$&</span>');
  text = text.replace(types, '<span data-style="orange">$&</span>');
  text = text.replace(classNames, '<span data-style="blue">$&</span>');
  text = text.replace(functionNames, '<span data-style="cyan">$&</span>');
  text = text.replace(brackets, '<span data-style="yellow">$&</span>');

  return text;
}
// document.getElementById("Akzestia-x").addEventListener("click", function () {
//   render("Akzestia");
// });
document.getElementById("Client-x").addEventListener("click", function () {
  render("Client");
});
document.getElementById("Server-x").addEventListener("click", function () {
  render("Server");
});
document.getElementById("Wayland-x").addEventListener("click", function () {
  render("Wayland");
});
document.getElementById("X11-x").addEventListener("click", function () {
  render("X11");
});
document.getElementById("Qt-x").addEventListener("click", function () {
  render("Qt");
});
document.getElementById("MsQuic-x").addEventListener("click", function () {
  render("MsQuic");
});
document.getElementById("Linux-x").addEventListener("click", function () {
  render("Linux");
});
document.getElementById("Windows-x").addEventListener("click", function () {
  render("Windows");
});
document.getElementById("Dev-Team-x").addEventListener("click", function () {
  render("Dev Team");
});
document.getElementById("FAQ-x").addEventListener("click", function () {
  render("FAQ");
});
