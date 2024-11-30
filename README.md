# Varasto server C++ implementation

C++17 implementation of [Varasto server]. Work in progress.

[Varasto server]: https://www.npmjs.com/package/@varasto/server

## Compilation

```bash
$ git submodule update --init
$ mkdir build
$ cd build
$ cmake ..
$ make
```

## Usage

Create directory where the data will stored into, then launch `varasto-server`
with the directory as argument, such as:

```bash
$ mkdir data
$ varasto-server ./data
```

By default port `8080` will be used. This can be overridden with `-p` switch.

### Storing items

To store an item, you can use a `POST` request like this:

```http
POST /foo/bar HTTP/1.0
Content-Type: application/json
Content-Length: 14

{"foo": "bar"}
```

Or you can use [curl] to store an item like this:

```bash
$ curl -X POST \
    -H 'Content-Type: application/json' \
    -d '{"foo": "bar"}' \
    http://localhost:8080/foo/bar
```

[curl]: https://curl.haxx.se/

If you want an key to the entry to be automatically generated (it will be
[UUID]) you can omit the key from the request like this:

```http
POST /foo HTTP/1.0
Content-Type: application/json
Content-Length: 14

{"foo": "bar"}
```

And you get an response like this that contains the automatically generated
key:

```json
{ "key": "13aa0984-af0f-11ef-a02b-2743ddb77e05" }
```

[UUID]: https://en.wikipedia.org/wiki/Universally_unique_identifier

### Retrieving items

To retrieve a previously stored item, you make an `GET` request, where the
request path again acts as the identifier of the item.

```http
GET /foo/bar HTTP/1.0
```

To which the HTTP server will respond with the JSON object previously stored
with namespace `foo` and key `bar`. If an item with given key under the given
namespace does not exist, HTTP error 404 will be returned instead.

### Listing items

To list all items stored under an namespace, you make an `GET` request with
name of the namespace as the request path.

```http
GET /foo HTTP/1.0
```

To which the HTTP server will respond with an JSON object which contains each
item stored under namespace foo mapped with the key that they were stored with.

```json
{
  "bar": {
    "foo": "bar"
  }
}
```

### Removing items

To remove an previously stored item, you make a `DELETE` request with the
request path again acting as the identifier of the item you wish to remove.

```http
DELETE /foo/bar HTTP/1.0
```

If item with key bar under namespace foo exists, it's value will be returned
as response. If such item does not exist, HTTP error 404 will be returned
instead.

### Removing namespaces

To remove all entries stored under an namespace, you make a `DELETE` request
with the request path acting as identifier of the namespace you wish to remove.

```http
DELETE /foo HTTP/1.0
```

If an namespace with the given identifier exists, an object containing all the
entries that existed in the namespace will be returned as response. If such
namespace does not exist, HTTP error 404 will be returned instead.

### Updating items

You can also partially update an already existing item with `PATCH` request.
The JSON sent with an PATCH request will be shallowly merged with the already
existing data and the result will be sent as response.

For example, you have an item john-doe under namespace people with the following data:

```json
{
  "name": "John Doe",
  "address": "Some street 4",
  "phoneNumber": "+35840123123"
}
```

And you send an `PATCH` request like this:

```http
PATCH /people/john-doe HTTP/1.0
Content-Type: application/json
Content-Length: 71

{
  "address": "Some other street 5",
  "faxNumber": "+358000000"
}
```

You end up with:

```json
{
  "name": "John Doe",
  "address": "Some other street 5",
  "phoneNumber": "+35840123123",
  "faxNumber": "+358000000"
}
```

## TODO

- Caching.
- SSL support.
- Basic authentication support.
