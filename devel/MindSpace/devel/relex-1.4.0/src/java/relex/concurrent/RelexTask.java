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

package relex.concurrent;

import java.util.concurrent.BlockingQueue;
import java.util.concurrent.Callable;

import relex.ParsedSentence;
import relex.Sentence;
import relex.algs.SentenceAlgorithmApplier;
import relex.entity.EntityMaintainer;
import relex.tree.PhraseMarkup;
import relex.tree.PhraseTree;

/**
 * Processes a sentence using the given LinkParserClient. When processing is
 * finished, returns the LPC to the pool. 
 *  
 * @author muriloq
 */
public class RelexTask implements Callable<RelexTaskResult>
{
	public static final int DEBUG = 0;
	// arguments 
	private int index;
	private String sentence;
	private EntityMaintainer entityMaintainer;
	
	// Reusable, shared processors 
	private SentenceAlgorithmApplier sentenceAlgorithmApplier;
	private PhraseMarkup phraseMarkup;

	// Used in mutual exclusion, must be returned to the pool
	private RelexContext context; 
	private BlockingQueue<RelexContext> pool;
	
	public RelexTask(int index, String sentence,
			EntityMaintainer entityMaintainer, 
			SentenceAlgorithmApplier sentenceAlgorithmApplier,
			PhraseMarkup phraseMarkup,
			RelexContext context, BlockingQueue<RelexContext> pool){
		this.index = index;
		this.entityMaintainer = entityMaintainer; 
		this.sentenceAlgorithmApplier = sentenceAlgorithmApplier;
		this.phraseMarkup = phraseMarkup;
		this.context = context;
		this.pool = pool;
		this.sentence = sentence;
	}
	
	public RelexTaskResult call() {
		try {
			if (DEBUG > 0) System.err.println("[" + index + "] Start processing "+ sentence);
			entityMaintainer.convertSentence(sentence,null);
			String convertedSentence = entityMaintainer.getConvertedSentence().replace('\n', ' ').replace('\r', ' ');
			if (DEBUG > 0) System.err.println("[" + index + "] End entity detection");
			Sentence sntc = null;
			try {
				sntc = context.getParser().parse(convertedSentence);//, context.getLinkParserClient());
			} catch (RuntimeException ex) {
				sntc = new Sentence();
				sntc.setSentence(sentence);
			}
			
			if (DEBUG > 0) System.err.println("[" + index + "] End parsing");

			int i = 0;
			for (ParsedSentence parse : sntc.getParses()) {
				try {
					// Markup feature node graph with entity info,
					// so that the relex algs (next step) can see them.
					entityMaintainer.tagConvertedSentence(parse);

					// The actual relation extraction is done here.
					sentenceAlgorithmApplier.applyAlgs(parse, context);

					// Strip out the entity markup, so that when the
					// sentence is printed, we don't print gunk.
					entityMaintainer.repairSentence(parse.getLeft());

					// Also do a Penn tree-bank style phrase structure markup.
					if (phraseMarkup != null)
					{
						phraseMarkup.markup(parse);
						// Repair the entity-mangled tree-bank string.
						PhraseTree pt = new PhraseTree(parse.getLeft());
						parse.setPhraseString(pt.toString());
					}
				} catch (Exception e) {
					e.printStackTrace();
				}
				if (DEBUG > 0) 
					System.err.println("[" + index+ "] end post-processing sentence " + 
							(i++) + "/"+ sntc.getParses().size());
			}
			return new RelexTaskResult(index, sentence, entityMaintainer, sntc);
		} finally {
			if (DEBUG > 0)
				System.err.println("[" + index + "] End processing");
			try {
				pool.put(context);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
			if (DEBUG > 0) System.err.println("[" + index + "] Release resources");
		}
	}
	
	public String toString()
	{
		return index + ": " + sentence;
	}
}
