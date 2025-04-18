%%
%% %CopyrightBegin%
%%
%% SPDX-License-Identifier: Apache-2.0
%%
%% Copyright Ericsson AB 2004-2025. All Rights Reserved.
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
%%

%%
%% ct:run("../inets_test", uri_SUITE).
%%

-module(uri_SUITE).

-include_lib("eunit/include/eunit.hrl").
-include_lib("common_test/include/ct.hrl").
-include("inets_test_lib.hrl").

%% Note: This directive should only be used in test suites.
-compile(export_all).

-define(GOOGLE, "www.google.com").

%%--------------------------------------------------------------------
%% Common Test interface functions -----------------------------------
%%--------------------------------------------------------------------
suite() ->
    [{ct_hooks,[ts_install_cth]}].

all() ->
    [encode_decode].

%%--------------------------------------------------------------------

init_per_suite(Config) ->
    Config.
end_per_suite(_Config) ->
    ok.

%%--------------------------------------------------------------------

init_per_testcase(_Case, Config) ->
    Config.
end_per_testcase(_Case, _Config) ->
    ok.

%%-------------------------------------------------------------------------
%% Test cases starts here.
%%-------------------------------------------------------------------------


encode_decode(Config) when is_list(Config) ->
    ?assertEqual("foo%20bar", http_uri:encode("foo bar")),
    ?assertEqual(<<"foo%20bar">>, http_uri:encode(<<"foo bar">>)),

    ?assertEqual("foo+bar", http_uri:decode("foo+bar")),
    ?assertEqual(<<"foo+bar">>, http_uri:decode(<<"foo+bar">>)),
    ?assertEqual("foo bar", http_uri:decode("foo%20bar")),
    ?assertEqual(<<"foo bar">>, http_uri:decode(<<"foo%20bar">>)),
    ?assertEqual("foo\r\n", http_uri:decode("foo%0D%0A")),
    ?assertEqual(<<"foo\r\n">>, http_uri:decode(<<"foo%0D%0A">>)).
