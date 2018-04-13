#!/bin/bash

dot -Tpdf graph.dot -o graph.pdf
evince graph.pdf
