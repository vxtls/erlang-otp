%%
%% %CopyrightBegin%
%%
%% SPDX-License-Identifier: Apache-2.0
%%
%% Copyright Ericsson AB 1999-2025. All Rights Reserved.
%%
%% Licensed under the Apache License, Version 2.0 (the "License");
%% you may not use this file except in compliance with the License.
%% You may obtain a copy of the License at
%%
%%     http://www.apache.org/licenses/LICENSE-2.0
%%
%% Unless required by applicable law or agreed to in writing, software
%% distributed under the License is distributed on an "AS IS" BASIS,
%% WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
%% See the License for the specific language governing permissions and
%% limitations under the License.
%%
%% %CopyrightEnd%
%%

-module(bs_match_tail_SUITE).

-author('bjorn@erix.ericsson.se').
-export([all/0, suite/0,
	 aligned/1,unaligned/1,zero_tail/1,huge_tail/1]).

-include_lib("common_test/include/ct.hrl").

suite() -> [{ct_hooks,[ts_install_cth]}].

all() -> 
    [aligned, unaligned, zero_tail, huge_tail].


%% Test aligned tails.
aligned(Config) when is_list(Config) ->
    Tail1 = mkbin([]),
    {258,Tail1} = al_get_tail_used(mkbin([1,2])),
    Tail2 = mkbin(lists:seq(1, 127)),
    {35091,Tail2} = al_get_tail_used(mkbin([137,19|Tail2])),

    64896 = al_get_tail_unused(mkbin([253,128])),
    64895 = al_get_tail_unused(mkbin([253,127|lists:seq(42, 255)])),

    Tail3 = mkbin(lists:seq(0, 19)),
    {0,Tail1} = get_dyn_tail_used(Tail1, 0),
    {0,Tail3} = get_dyn_tail_used(mkbin([Tail3]), 0),
    {73,Tail3} = get_dyn_tail_used(mkbin([73|Tail3]), 8),

    0 = get_dyn_tail_unused(mkbin([]), 0),
    233 = get_dyn_tail_unused(mkbin([233]), 8),
    23 = get_dyn_tail_unused(mkbin([23,22,2]), 8),
    ok.

al_get_tail_used(<<A:16,T/binary>>) -> {A,T}.
al_get_tail_unused(<<A:16,_/binary>>) -> A.

%% Test that an non-aligned tail cannot be matched out.
unaligned(Config) when is_list(Config) ->
    {'EXIT',{function_clause,_}} = (catch get_tail_used(mkbin([42]))),
    {'EXIT',{{badmatch,_},_}} = (catch get_dyn_tail_used(mkbin([137]), 3)),
    {'EXIT',{function_clause,_}} = (catch get_tail_unused(mkbin([42,33]))),
    {'EXIT',{{badmatch,_},_}} = (catch get_dyn_tail_unused(mkbin([44]), 7)),
    ok.

get_tail_used(<<A:1,T/binary>>) -> {A,T}.

get_tail_unused(<<A:15,_/binary>>) -> A.

get_dyn_tail_used(Bin, Sz) ->
    <<A:Sz,T/binary>> = Bin,
    {A,T}.

get_dyn_tail_unused(Bin, Sz) ->
    <<A:Sz,_/binary>> = Bin,
    A.

%% Test that zero tails are tested correctly.
zero_tail(Config) when is_list(Config) ->
    7 = (catch test_zero_tail(mkbin([7]))),
    {'EXIT',{function_clause,_}} = (catch test_zero_tail(mkbin([1,2]))),
    {'EXIT',{function_clause,_}} = (catch test_zero_tail2(mkbin([1,2,3]))),
    ok.

test_zero_tail(<<A:8>>) -> A.

test_zero_tail2(<<_A:4,_B:4>>) -> ok.

huge_tail(_Config) ->
    42 = huge_tail_1(id(<<42,0:16#1001>>)),
    {'EXIT',{function_clause,_}} = catch huge_tail_1(id(<<0:8,0:10>>)),

    {'EXIT',{function_clause,_}} = catch huge_tail_2(id(<<0:8,0:100>>)),

    {'EXIT',{function_clause,_}} = catch huge_tail_3(id(<<0:8,0:200>>)),

    %% The following code is commented out by default because it
    %% constructs a 2Gb binary.

    %% try huge_tail_2(id(<<100, 0:16#8000_0000>>)) of
    %%     100 ->
    %%         ok
    %% catch
    %%     error:function_clause ->
    %%         ct:fail(not_supposed_to_fail)
    %% end,

    ok.

%% On AArch64, the immediate for the `cmp` instruction is 12 bits (unsigned).
huge_tail_1(<<B:8,_:16#1001>>) -> B.

%% On x86_64, the immediate for the `cmp` instruction is 32 bits (signed).
huge_tail_2(<<B:8, _:16#8000_0000>>) -> B.

%% Matching will fail on all platforms.
huge_tail_3(<<B:8, _:16#1_0000_0000_0000_0000>>) -> B.

%%%
%%% Common utilities.
%%%
mkbin(L) when is_list(L) -> list_to_binary(id(L)).

id(I) -> I.
