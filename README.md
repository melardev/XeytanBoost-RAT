
# WARNING
This application is under development, it is not finished, nor stable.
# Introduction
RAT tool written in C++ using the Boost libraries. At this moment in time it is console based.
In the future I will be using Boost.Ui for the graphical user interface.

# Features
- Get Client Info
- Process List
- Reverse Shell
- FileSystem explorer

# Design considerations
## shared_ptr<void>
First of all, yeah, it works, and this is why [1](http://shanekirk.com/2017/11/c-stdshared_ptrvoid/) 
and [2](https://stackoverflow.com/questions/5913396/why-do-stdshared-ptrvoid-work).<br>
If you are not happy with this pattern,the solution would be to create a class event per different use, 
ending with a lot of classes.<br>
Each event class would have the "strongly typed" field you actually use for example `shared_ptr<string>`.
For now I save me time and use a few classes, but having `shared_ptr<void>` so they can hold anything.

# TODO
- Improve the packet sending algorithm, at this moment in time we send everything in one single thread
and one packet at a time to avoid concurrency issues, but the synchronization has only to be
enformed per client, so one packet per client at a time, and not one packet per thread.
We could deliver many packets at the same time as long we are not already sending a packet for
that same client, this would improve performance. Easy to implement.
- Implement security feature: keep track of expected packets, discard any packet that is not expected.
For example if we received a process packet when we did not expect that, we should discard it. Easy to implement.
- Convert Process::icon into ascii art to be rendered in the console, there are many snippets
in the internet that could be used.
- Process List feature: Extract icons from file paths.

