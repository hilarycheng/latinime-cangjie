package com.diycircuits.cangjie;

public interface CandidateListener {

    void characterSelected(char c, int idx);
    void phraseSelected(String phrase, int idx);

}
    
