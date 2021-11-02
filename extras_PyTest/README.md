Introduction
============

The real prototyping with OpenCascade starts in Python. Although the original developers
of OpenCascade thought it's gonna be Tcl (DRAW Test Harness application), that actually
never happened. Since C++ language is hard to get started with, OpenCascade remained
largely closed for scientific and engineering communities, because people in those
communities are not software developers.

Thanks the folks from OpenCascade community, we got a bunch of options for
prototyping in Python, and that is the real prototyping. Once you're done with
your MVP in Python, you can consider moving your development into C++ for the
sake of efficiency.

Personally, I had zero experience in Python. The only reason I started to look
into it was because there were so many people using PythonOCC that it was impossible
to continue ignoring this whole Python business.

In what follows, I try explain how to get started with PythonOCC having Anaconda3
as a runtime environment.

What is PythonOCC
=================

PythonOCC (https://www.pythonocc.org/) is a project driven by Thomas Paviot.
Let's give a word to Zoltan (our forum member):

"PythonOCC is merely a wrapper (with some additional utilities) around OpenCASCADE, so that
 other applications can build higher-level features... However, PythonOCC wraps almost the
 entire OpenCASCADE, preserving the function signatures, so you can search for help on the
 C++ forums and apply them straightforwardly in your Python application. PythonOCC uses SWIG
 for exposing OpenCASCADE to Python."

More on Anaconda
================

First of all, visit and carefully follow the instructions given by Alexander
in this forum topic: http://analysissitus.org/forum/index.php?threads/pythonocc-getting-started-guide.19/

After reading this topic and following the described steps you should end up having the working Anaconda
distribution and PythonOCC installed in there. What you read on the forum is a quick guide that does not
explain what Anaconda is and how all this stuff works under the hood. So if you want to dive deeper, you
can follow some of the many videos/tutorials listed in the "Learning" tab of Anaconda Navigator. E.g.:

- Conda Deep Dive by Kale Franz: https://www.youtube.com/watch?v=zKejVMgwJuE

If you're curious, you can navigate to the directory where PythonOCC Anaconda environment and package
are located. You can find all the packages installed here (Windows):

/ProfileDir/\.conda\pkgs

A bunch of files located in a directory make this directory an Anaconda package:

- info/index.json is the description of the package.
- info/paths.json is the metadata about the individual files in the package. Those files end up installing
                  in the environment.
- info/link.json  is the optional install-time metadata.
