.. SBF documentation master file, created by
   sphinx-quickstart on Tue Nov 21 11:09:34 2017.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Super Bloody Fast transport system!
===================================
SBF is a message transport system, written in C, that allows inter app communication in two different flavours:
* Publisher - Subscriber
* Requester - Replier

The `publisher-subscriber` consists of publishers defining topics where messages are placed. Then one or more subscribers might register to a concrete topic to read the messages that were delivered (one to many communication).
On the other hand, the `requester-replier` communication is about defining a channel where messages are sent from one requester to one publisher (one to one communication).

.. toctree::
   :maxdepth: 2
   :caption: Contents:

Indices and tables
==================

* :ref:`genindex`
* :ref:`search`

API
===

.. doxygenindex::
   :project: sbf
