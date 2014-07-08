h1. KDE client coding guidelines

This document document the coding style

h2. Coding style

* Indentation using 3 spaces
* Every function/method need a oneliner doxygen description using "///"
* Function need to be ordered by 1) usage (history, contact, slots) 2) alphabetical order
* Lines related to each other (like many setter on the same object need to be aligned down to the ";"
* Oneliner if-elseif/switch/connect/tables need to have an header using /* HEADER */ where */ close of the end of the table, also align the lines
* Header file function need to be classified and aligned
* License header is necessary for all source files
* Attribute are named with prefix "m_", then "s" for static, then "p" for pointer, "l" for list, "h" for hash, "f" for function/lambdas then the first letter is uppercase
* Includes are located in the .cpp and only "class" and "namespace" are declared in the .h (where applicable)
* Includes are sorted by libraries (Qt -> QtCore/QtGui/QtDbus, KDE -> Akonadi/KABC, SFLPhone)
* Setters take const references as parameter
* Destructor are always virtual
* C++11 is allowed
* Every class need a oneliner "///@class <Classname> <description>" description
* Use QDebug
* Align everything as much as possible
* one line "if" block and "()?:;" are allowed and recommended
* Minimize the number of lines, but add as many white lines are necessary
* Header ifnedef need to be the class upper name with upper replaced with _ and _H as suffix
* functions/methods need to end with } //function name (more than 10 lines only)
* "using" (namespace keyword) usage is prohibited 
* Every #endif need to make explicit what it is closing "#endif MY_CLASS_H"
* Code block have the "{" at the end of line while mothods use "\n{"
* Use enum class where possible
* Always use the Full::Path for enums
* Expose library objects as "real" qobjects (with Q_PROPERTY, Q_ENUM, Q_FLAGS, Q_INTERFACE and Q_DECLARE_NAME)
* Follow Qt getter/setter naming conventions
* Follow Krazy2 advices
* string should be stored in a static struct or class as "constexpr static const char*". C++14 will add some useful awesomeness with this
* daemon constants should always be stored in the .h in pure static structs (with embedded structs to emulate namespace)
* avoid keeping maps and list around when they represent an object.
* transform string into enum class literal or object as soon as possible in the code
* use const and static variables in methods where applicable
* Classes should not expose multiple mutually exclusive boolean properties and use enum classes instead. (Example: isAudioOnly() and hasVideo())

h2. Design guideline

* No hack
* No logic that belong to the daemon
* Use the state machine, don't add useless functions
* Coherent code is not a very high priority, but avoid making things worst
* Avoid using dbus call outside of the library, if you have to, your doing it wrong


h2. Current design

<pre>
=========================QML plasmoid====== <- Abstact frontend using the dataengine model
=====KDE frontend=========Dateengine======= <- Library frontend
==============KDE library================== <- Common GUI code and contact implementation
===============Qt library================== <- Dbus to C++ object + state machine + frontend model, -GUI LESS-
==================DBUS=====================
================SFLPhone===================
=================PJSIP=====================
==================UDP======================
================SERVER=====================
</pre>
