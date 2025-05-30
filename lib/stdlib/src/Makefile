#
# %CopyrightBegin%
#
# SPDX-License-Identifier: Apache-2.0
#
# Copyright Ericsson AB 1996-2025. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# %CopyrightEnd%
#

ifdef BOOTSTRAP
EGEN=$(BOOTSTRAP_TOP)/lib/stdlib/egen
EBIN=$(BOOTSTRAP_TOP)/lib/stdlib/ebin
endif

include $(ERL_TOP)/make/target.mk
include $(ERL_TOP)/make/$(TARGET)/otp.mk

# ----------------------------------------------------
# Application version
# ----------------------------------------------------
include ../vsn.mk
VSN=$(STDLIB_VSN)

# ----------------------------------------------------
# Release directory specification
# ----------------------------------------------------
RELSYSDIR = $(RELEASE_PATH)/lib/stdlib-$(VSN)

# ----------------------------------------------------
# Target Specs
# ----------------------------------------------------
MODULES= \
	array \
	argparse \
	base64 \
	beam_lib \
	binary \
	c \
	calendar \
	dets \
	dets_server \
	dets_sup \
	dets_utils \
	dets_v9 \
	dict \
	digraph \
	digraph_utils \
	edlin \
	edlin_key \
	edlin_context \
	edlin_expand \
	edlin_type_suggestion \
	epp \
	erl_abstract_code \
	erl_anno \
	erl_bits \
	erl_compile \
	erl_error \
	erl_eval \
	erl_expand_records \
	erl_features \
	erl_internal \
	erl_lint \
	erl_parse \
	erl_posix_msg \
	erl_pp \
	erl_scan \
	erl_stdlib_errors \
	erl_tar \
	error_logger_file_h \
	error_logger_tty_h \
	escript \
	ets \
	eval_bits \
	file_sorter \
	filelib \
	filename \
	gb_trees \
	gb_sets \
	gen \
	gen_event \
	gen_fsm \
	gen_server \
	gen_statem \
	io \
	io_lib \
	io_lib_format \
	io_lib_fread \
	io_lib_pretty \
	json \
	lists \
	log_mf_h \
	maps \
	math \
	ms_transform \
	otp_internal \
	orddict \
	ordsets \
	re \
	peer \
	pool \
	proc_lib \
	proplists \
	qlc \
	qlc_pt \
	queue \
	rand \
	random \
	sets \
	shell \
	shell_default \
	shell_docs \
	shell_docs_markdown \
	shell_docs_test \
	slave \
	sofs \
	string \
	supervisor \
	supervisor_bridge \
	sys \
	timer \
	unicode \
	unicode_util \
	uri_string \
	win32reg \
	zip \
	zstd

HRL_FILES= \
	../include/assert.hrl \
	../include/erl_compile.hrl \
	../include/erl_bits.hrl \
	../include/ms_transform.hrl \
	../include/qlc.hrl \
	../include/zip.hrl

INTERNAL_HRL_FILES= dets.hrl erl_tar.hrl otp_internal.hrl json.hrl

ERL_FILES= $(MODULES:%=%.erl)

TARGET_FILES= $(MODULES:%=$(EBIN)/%.$(EMULATOR)) $(APP_TARGET) $(APPUP_TARGET)

ERL_ID_TRANS=../examples/erl_id_trans.erl

APP_FILE= stdlib.app

APP_SRC= $(APP_FILE).src
APP_TARGET= $(EBIN)/$(APP_FILE)

APPUP_FILE= stdlib.appup

APPUP_SRC= $(APPUP_FILE).src
APPUP_TARGET= $(EBIN)/$(APPUP_FILE)

ifeq ($(TARGET),win32)
  EXE_SUFFIX=.exe
else
  EXE_SUFFIX=
endif

# ----------------------------------------------------
# FLAGS
# ----------------------------------------------------

ERL_COMPILE_FLAGS += -Werror  +warn_deprecated_catch
ERL_COMPILE_FLAGS += -I../include -I../../kernel/include

ifeq ($(ERL_DETERMINISTIC),yes)
	DETERMINISM_FLAG = +deterministic
else
	DETERMINISM_FLAG =
endif

# ----------------------------------------------------
# Targets
# ----------------------------------------------------

$(TYPES): $(TARGET_FILES)

clean:
	rm -f $(TARGET_FILES)
	rm -f core
	rm -f erl_parse.erl

docs:

# This is a trick so that the preloaded files will get the correct type
# specifications.
primary_bootstrap_compiler: \
  $(BOOTSTRAP_COMPILER)/ebin/epp.beam \
  $(BOOTSTRAP_COMPILER)/ebin/erl_anno.beam \
  $(BOOTSTRAP_COMPILER)/ebin/erl_error.beam \
  $(BOOTSTRAP_COMPILER)/ebin/erl_features.beam \
  $(BOOTSTRAP_COMPILER)/ebin/erl_scan.beam \
  $(BOOTSTRAP_COMPILER)/ebin/erl_parse.beam \
  $(BOOTSTRAP_COMPILER)/ebin/erl_lint.beam \
  $(BOOTSTRAP_COMPILER)/ebin/io.beam \
  $(BOOTSTRAP_COMPILER)/ebin/otp_internal.beam \
  $(BOOTSTRAP_COMPILER)/ebin/erl_internal.beam

opt: $(EBIN)/erl_id_trans.beam

$(BOOTSTRAP_COMPILER)/ebin/erl_parse.beam: erl_parse.yrl
	$(gen_verbose)
	$(V_at)$(ERLC) -o $(BOOTSTRAP_COMPILER)/egen $(DETERMINISM_FLAG) erl_parse.yrl
	$(V_at)$(ERLC) -o $(BOOTSTRAP_COMPILER)/ebin $(DETERMINISM_FLAG) $(BOOTSTRAP_COMPILER)/egen/erl_parse.erl

$(BOOTSTRAP_TOP)/lib/stdlib/egen/erl_parse.erl: erl_parse.yrl
	$(yecc_verbose)$(ERLC) $(YRL_FLAGS) -o$(BOOTSTRAP_TOP)/lib/stdlib/egen erl_parse.yrl

$(BOOTSTRAP_COMPILER)/ebin/%.beam: %.erl
	$(V_ERLC) -o $(BOOTSTRAP_COMPILER)/ebin $(DETERMINISM_FLAG) $<

$(EBIN)/erl_id_trans.beam: ../examples/erl_id_trans.erl
	$(V_ERLC) $(ERL_COMPILE_FLAGS) -o $(EBIN) $<

# ----------------------------------------------------
# Special Build Targets
# ----------------------------------------------------

$(APP_TARGET): $(APP_SRC) ../vsn.mk
	$(vsn_verbose)sed -e 's;%VSN%;$(VSN);' $< > $@

$(APPUP_TARGET): $(APPUP_SRC) ../vsn.mk
	$(vsn_verbose)sed -e 's;%VSN%;$(VSN);' $< > $@

unicode_util.erl: ../uc_spec/*
	escript$(EXE_SUFFIX) ../uc_spec/gen_unicode_mod.escript

# ----------------------------------------------------
# Release Target
# ----------------------------------------------------
include $(ERL_TOP)/make/otp_release_targets.mk

release_spec: opt
	$(INSTALL_DIR) "$(RELSYSDIR)/src"
	$(INSTALL_DATA) $(ERL_FILES) erl_parse.yrl "$(RELSYSDIR)/src"
	$(INSTALL_DATA) $(INTERNAL_HRL_FILES) "$(RELSYSDIR)/src"
	$(INSTALL_DIR) "$(RELSYSDIR)/include"
	$(INSTALL_DATA) $(HRL_FILES) "$(RELSYSDIR)/include"
	$(INSTALL_DIR) "$(RELSYSDIR)/ebin"
	$(INSTALL_DATA) $(TARGET_FILES) "$(RELSYSDIR)/ebin"

release_docs_spec:

# ----------------------------------------------------
# Dependencies
# ----------------------------------------------------
include $(ERL_TOP)/make/dep.mk
