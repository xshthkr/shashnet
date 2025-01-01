# Shashnet

![Compilation](https://github.com/xshthkr/shashnet/actions/workflows/c-cpp.yml/badge.svg)

A custom application layer protocol for reliable, ordered and lossless data transfer over UDP.

## Architecture

### Server

Thread 1: Welcoming socket received incoming packets

![alt text](<assets/server-thread 1.drawio.png>)

Thread 2: Broadcast messages to clients

![alt text](<assets/server-thread 2.drawio.png>)

## Resources

- [UDP (RFC 768)](https://www.ietf.org/rfc/rfc768.txt)
- [TCP (RFC 793)](https://www.ietf.org/rfc/rfc793.txt)
- [Principles of Relible Data Transfer - Keith W. Ross and James F. Kurose](http://www2.ic.uff.br/~michael/kr1999/3-transport/3_040-principles_rdt.htm)
