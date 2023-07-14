# Q2: Does the C++ driver implement the standard API or index view API for indexes?

A:

The index view API is implemented: https://jira.mongodb.org/browse/CXX-1358
The standard API is partially implemented: https://jira.mongodb.org/browse/CXX-1351

# Q1: Why is a runtime error observed on Windows?

A: 
See https://jira.mongodb.org/browse/CXX-1846?focusedCommentId=2506398&page=com.atlassian.jira.plugin.system.issuetabpanels%3Acomment-tabpanel#comment-2506398:

> A developer only hint: on Windows, very frequently these sorts of weird crashes are caused by inadvertently mixing different runtime libraries. All of the components (C driver, Boost, C++ Driver, Application) must agree on whether the Debug or Release CRT is in play, along with whether you are building against the Static or Dynamic version is being used. Weird string crashes are the canonical symptom of such misconfigurations.
