# Contributing

Thank you for your interest in FINAL CUT!

We welcome contributions of all kinds, including patches to code and 
documentation, bug reports, or just kind feedback. If you ever think that 
something could be better or different, please give us feedback on it. Do 
you have problems with FINAL CUT, or are you looking for support in using 
the library in your project? Feel free to open a new issue if you don't 
see one. If you have problems with an application that uses FINAL CUT 
and you are unsure of the cause, please contact the author first.

The FINAL CUT source code is hosted on GitHub; you can clone the repository 
to create a local copy:

```bash
> git clone https://github.com/gansm/finalcut.git
```


## Submitting patches

The FINAL CUT project accepts both 
[GitHub pull requests](https://help.github.com/en/github/collaborating-with-issues-and-pull-requests/creating-a-pull-request) 
and patches that are sent via via other channels. GitHub pull requests are 
preferred. Please follow our [coding style](doc/coding-style.txt) standard.


### Explain your work

When submitting a patch, you should include a description of the problem 
you are trying to solve, how you solved it, and why you chose the solution 
you implemented. When you submit a bug fix, it is often helpful to include 
a reproducible description of the problem in your explanation. It would 
also be helpful to include instructions on how to verify that the issue 
has been resolved.


### Test your code

Run the FINAL CUT unit tests if you have installed 
[CppUnit](https://freedesktop.org/wiki/Software/cppunit/). 
Follow these steps:

The easy way

```bash
> make distclean
> ./build.sh unit-test
```

or

```bash
> make distclean
> ./configure --prefix=/usr CXXFLAGS="-g -O0 -DDEBUG -DUNIT_TEST" --with-unit-test
> make test
```

If your changes aren't fully covered, I’d appreciate any additional test cases.


## Issue Tracking

We currently use the 
[issue tracker on Github](https://github.com/gansm/finalcut/issues).


## How to help

### What FINAL CUT needs

* We want your feedback, your opinion, your advice, and to hear what you 
think!
* Help make FINAL CUT better known. Tell others about this project. A large 
user base also means a better and more stable codebase.
* Help maintain and improve the library.
* Write unit tests for untested code.

### What would be helpful

* **Improve documentation:** <br />
Fix typos or grammar mistakes. Revise sections that need improvement or 
add missing sections.
* **Translations into other languages:** <br />
For many people, reading the documentation in their native language will 
certainly make getting started with FINAL CUT much easier and more 
attractive.
* **Better tests:** <br />
High code coverage provides a stable base that prevents code from breaking 
due to minor changes.
* **Adding to repositories:** <br />
Advocate for the inclusion of FINAL CUT in your preferred distribution.
* **Support for other platforms:** <br />
There are still platforms that are not yet supported
* **Would you like to help other people?** <br />
Answer questions in open issues or help moderate discussion forums or 
other communication channels.

Please help us make the FINAL CUT widget toolkit the best library for 
text-based user interfaces.

