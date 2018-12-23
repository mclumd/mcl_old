#!/bin/sh

if [ $# -ne 1 ]; then
	echo 1>&2 "Usage: $0 <new_domain_name>"
	exit 127
fi

mkdir domains/$1

mkdir domains/$1/agents
mkdir domains/$1/env
mkdir domains/$1/controllers
mkdir domains/$1/scorers
mkdir domains/$1/common
mkdir domains/$1/utils
mkdir domains/$1/documentation

mkdir domains/$1/agents/include
mkdir domains/$1/env/include
mkdir domains/$1/controllers/include
mkdir domains/$1/scorers/include
mkdir domains/$1/common/include
mkdir domains/$1/utils/include

cp assets/initMCL.cc.template domains/$1/utils/initMCL.cc
cp assets/Make.domain.template domains/$1/Makefile
cp assets/main_domain_template.cc domains/$1/main.cc
