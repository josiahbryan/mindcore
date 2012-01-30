/*
 * Copyright 2008 Novamente LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * The PatternCallback is an interface class.
 * It provides a a simple, easy callback interface 
 * to performing pattern matching on trees.
 *
 *  Copyright (C) 2008 Linas Vepstas <linas@linas.org>
 */

package relex.tree;

public interface PatternCallback
{
	public void FoundCallback(String pattern, PhraseTree pt);
	public Boolean PMCallback(String pattern, PhraseTree pt);
}
