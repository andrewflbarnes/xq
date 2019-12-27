# xq

## About

A naive XML parser which converts XML data from a file to YML.

**Why?** Because then we can pipe into `yq` and more easily process it ;)

In the future it would be cool to support this directly or to pull this capablity into `yq` directly using `cgo`

At the moment this will only parse nodes, values and attributes.

A tree is constructed with each node as the name with
- Values stored under a meta label "\_text"
- Attributes stored under a meta label "\_attr" as KV pairs

## Build

libxml2 is used and required to build.
```bash
make
```

## Examples

### Basic

Running `xq test.xml` on the below content

```xml
<?xml version="1.0" encoding="UTF-8">
<Message xmlns="message"
         xmlns:meta="message:meta">
    <Greeting vocal="optional">Hello</Greeting>
    <meta:Greetee>World</meta:Greetee>
    <meta:Excited>true</meta:Excited>
</Message>
```

would give
```yml
Message:
  Greeting:
   _attrs:
    vocal: "optional"
   _text: "Hello"
  Greetee:
   _text: "World"
  Excited:
   _text: "true"
```

### Combinations

This can be easily combined with `yq` and `jq` e.g.
```bash
yq r -j <(./xq test.xml) Message.Greeting | jq
```
gives
```json
{
  "_attrs": {
    "vocal": "optional"
  },
  "_text": "Hello"
}
```
