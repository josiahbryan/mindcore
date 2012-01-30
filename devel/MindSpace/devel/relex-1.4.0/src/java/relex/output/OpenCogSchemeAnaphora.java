/*
 * Copyright 2009 Novamente LLC
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

package relex.output;

import java.util.ArrayList;
import java.util.HashMap;

import relex.ParsedSentence;
import relex.Sentence;
import relex.anaphora.Antecedents;
import relex.anaphora.Hobbs;
import relex.anaphora.history.SentenceHistory;
import relex.anaphora.history.SentenceHistoryFactory;
import relex.anaphora.history.SentenceHistoryFactory.HistoryEnum;
import relex.feature.FeatureNode;


/**
 * Implements opencog scheme output from hobbs anaphora resolution.
 *
 * @author fabricio <fabricio@vettalabs.com>
 * @author Linas Vepstas <linasvepstas@gmail.com>
 *
 */
public class OpenCogSchemeAnaphora
{
	// The sentence being examined.
	private Sentence sentence;

	private static final String predicateName = "anaphoric reference";

	private Antecedents antecedents;
	private Hobbs hobbs;

	public OpenCogSchemeAnaphora()
	{
		antecedents = new Antecedents();
		hobbs = new Hobbs(antecedents);
		hobbs.setHistory(SentenceHistoryFactory.create(HistoryEnum.DEFAULT));
	}

	public void setSentence(ParsedSentence s)
	{
		sentence = s.getSentence();
	}

	public void clear()
	{
		// Clears out the old anaphora candidates
		antecedents.clear();
	}

	public void setHistory(SentenceHistory history)
	{
		if(hobbs != null)
			hobbs.setHistory(history);
	}
	
	public String toString()
	{
		return printAnaphora();
	}

	private String printAnaphora()
	{
		hobbs.addParse(sentence);
		hobbs.resolve(sentence);

		String str = new String();
		HashMap<FeatureNode,ArrayList<FeatureNode>> ante_map = 
		        antecedents.getAntecedents();

		for (FeatureNode anap: ante_map.keySet())
		{
			String anap_str = anap.get("str").getValue();
			String anap_guid = anap.get("uuid").getValue();
			double rank = 1.0;
			for (FeatureNode tgt: ante_map.get(anap))
			{
				String tgt_str = tgt.get("str").getValue();
				String tgt_guid = tgt.get("uuid").getValue();

				// Set a truth value with strength of 1.0 and
				// a confidence of 1/(n+1) where n=1,2,3... etc.
				// Here, n is the "Hobbs distance", so that n=1 is the
				// closest anaphoric reference, n=2 is the next, etc.
				rank += 1.0;
				double confidence = 1.0 / rank;

				str += 
				   "; ante(" + anap_str + ", " + tgt_str + ")\n" +
				   "(EvaluationLink (stv 1 " + confidence +")\n" +
				   "   (ConceptNode \"" + predicateName + "\")\n" +
				   "   (ListLink \n" +
				   "      (WordInstanceNode \"" + anap_guid + "\" )\n"+
				   "      (WordInstanceNode \"" + tgt_guid + "\" )\n"+
				   "   )\n" +
				   ")\n";
			}
		}
		return str;
	}
}
