# Shashnet

![Compilation](https://github.com/xshthkr/shashnet/actions/workflows/c-cpp.yml/badge.svg)

A custom application layer protocol for reliable, ordered, and lossless data transfer over UDP.

## Overview

Shashnet provides a reliable data transfer mechanism over UDP, emulating TCP-like behavior entirely at the application layer. By implementing features such as error detection, packet ordering, retransmission, flow control, and duplicate handling, Shashnet creates a virtual transport layer that enables seamless, dependable communication across unreliable networks.

## Features

- **Reliability**: Ensures all packets are received without loss, even in unreliable network conditions.
- **Packet Ordering**: Maintains the order of data packets for accurate reassembly at the receiver.
- **Error Detection and Handling**: Employs checksum verification to detect and handle corrupted packets.
- **Flow Control**: Balances the sender's rate with the receiver's capacity to prevent overflows.
- **Retransmissions**: Implements timeouts and retransmissions for unacknowledged packets.
- **Duplicate Handling**: Detects and discards duplicate packets to maintain data integrity.

## Architecture

The system consists of two primary components:

### Server

The server handles incoming data packets from the client, validates them, and reassembles them in the correct order. Key tasks include:

- Receiving packets and verifying checksums to detect corruption.
- Maintaining a buffer to handle out-of-order packets and ensuring correct sequencing.
- Sending acknowledgment packets to inform the client about successful receptions.

### Client

The client initiates the communication by sending data packets to the server. Core responsibilities include:

- Splitting data into smaller packets and assigning sequence numbers.
- Managing a sliding window to control the flow of data.
- Retransmitting packets when acknowledgments are not received within a timeout period.
- Handling cumulative acknowledgments to optimize performance.

### Virtualization on the Application Layer

Shashnet simulates features of a transport layer protocol entirely within the application layer, leveraging UDP as the underlying communication channel. This includes:

1. **Custom Packet Format**:
   - A structured format containing sequence numbers, acknowledgment numbers, payloads, and checksums.
   - Sequence and acknowledgment numbers are used to track data flow and maintain order.

2. **Sliding Window Mechanism**:
   - A sender-side buffer manages the number of packets in flight, ensuring that retransmissions and new transmissions occur seamlessly.
   - The receiver-side buffer reorders out-of-sequence packets for proper assembly.

3. **Error Handling**:
   - Checksum algorithms detect corrupted packets, prompting retransmission requests.
   - Retransmissions are triggered by sender-side timeouts if acknowledgments are delayed.

4. **Flow Control**:
   - Acknowledgment-based mechanisms ensure the sender adapts to the receiver's processing capacity, preventing data loss due to buffer overflows.

5. **Duplicate Detection**:
   - Sequence numbers enable the receiver to discard redundant packets, ensuring data integrity.

This virtualization approach allows Shashnet to mimic TCP-like reliability while retaining UDP's flexibility, making it a powerful solution for custom data transfer applications.

## Resources

- [UDP (RFC 768)](https://www.ietf.org/rfc/rfc768.txt)
- [TCP (RFC 793)](https://www.ietf.org/rfc/rfc793.txt)
- [Principles of Reliable Data Transfer - Keith W. Ross and James F. Kurose](http://www2.ic.uff.br/~michael/kr1999/3-transport/3_040-principles_rdt.htm)

---

This project cost me my sanity. Never again.
