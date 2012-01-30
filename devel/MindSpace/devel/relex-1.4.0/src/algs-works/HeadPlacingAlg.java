package relex.algs;
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

import relex.concurrent.RelexContext;
import relex.feature.FeatureNode;

/**
 * This algorithm combines sequences of words which should be a single word
 * (proper names, and idioms like "at hand")
 */
public class HeadPlacingAlg extends TemplateMatchingAlg {

	static String subjectLabelRegex = "S[a-z\\*]*"; // a-z or '*'

	static String headLabelRegex = "[CW][a-z\\*]*"; // a-z or '*'

	static String allLabelRegex = "(" + subjectLabelRegex + ")|(" + headLabelRegex + ")";

	protected void applyTo(FeatureNode node, RelexContext context) {
	}

}
