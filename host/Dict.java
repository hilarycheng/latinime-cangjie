import java.awt.*;
import java.io.*;
import java.util.*;

public class Dict
{

    public static void main(String[] args) {
	try {
	    FileOutputStream fo = new FileOutputStream("./phrase.csv");
	    Font font = new Font("Droid Sans Fallback", 16, Font.PLAIN);
	    BufferedReader reader = new BufferedReader(new FileReader("./phrase.txt"));

	    ArrayList<Character> clist = new ArrayList<Character>();
	    HashMap<Character, HashMap<String, ArrayList<String>>> dword = new HashMap<Character, HashMap<String, ArrayList<String>>>();
	    HashMap<Character, ArrayList<String>> map = new HashMap<Character, ArrayList<String>>();
	    HashMap<Character, Integer> maxphrase = new HashMap<Character, Integer>();
	    boolean canDisplay = true;
	    String line = null;
	    int total = 0;
	    int max_phrase = 0;
	    do {
		line = reader.readLine();
		if (line == null)
		    break;
		// StringTokenizer token = new StringTokenizer(line);
		// if (token.countTokens() < 1)
		//     continue;
		// String phrase = token.nextToken();
		String phrase = line.trim();
		if (phrase.length() < 2)
		    continue;
		if (!Character.isDefined(phrase.charAt(0)))
		    continue;
		canDisplay = true;
		for (int count = 0; count < phrase.length(); count++) {
		    canDisplay = canDisplay & font.canDisplay(phrase.charAt(count));
		}
		if (!canDisplay)
		    continue;
		total = total + (phrase.length() * 2);
		// System.out.println(phrase);
		Character c = new Character(phrase.charAt(0));
		if (map.containsKey(c)) {
		    String p = phrase;
		    p = p.substring(1);
		    if (!map.get(c).contains(p))
			map.get(c).add(p);
		} else {
		    String p = phrase;
		    p = p.substring(1);
		    ArrayList<String> slist = new ArrayList<String>();
		    slist.add(p);
		    map.put(c, slist);
		    clist.add(c);
		}
		int phraselen = phrase.length();
		if (phraselen > max_phrase) max_phrase = phraselen;
		if (maxphrase.containsKey(c)) {
		    if (phraselen > maxphrase.get(c).intValue()) {
			maxphrase.put(c, new Integer(phraselen));
		    }
		} else {
		    maxphrase.put(c, new Integer(phraselen));
		}
		{
		    String key = phrase.substring(0, 1);
		    int value = (int) key.charAt(0);
		    String other = phrase.substring(1);
		    fo.write((value + "," + other + ",0," + (phrase.length() - 1) + "\n").getBytes("UTF-8"));
		}
		String dw = phrase.substring(0, 2);
		if (dw.length() >= 2) {
		    if (!dword.containsKey(c)) {
			HashMap<String, ArrayList<String>> slist = new HashMap<String, ArrayList<String>>();
			ArrayList<String> sl = new ArrayList<String>();
			sl.add(phrase);
			slist.put(dw, sl);
			dword.put(c, slist);
		    } else {
			if (dword.get(c).containsKey(dw)) {
			    dword.get(c).get(dw).add(phrase);
			} else {
			    ArrayList<String> sl = new ArrayList<String>();
			    sl.add(phrase);
			    dword.get(c).put(dw, sl);
			}
		    }
		}
	    } while (line != null);
	    // System.out.println(total);
	    // System.out.println(clist.size());
	    Character c = null;
	    int max = 0;
	    for (int count = 0; count < clist.size(); count++) {
		// System.out.println(clist.get(count) + " " + map.get(clist.get(count)).size());
		if (map.get(clist.get(count)).size() > max) {
		    max = map.get(clist.get(count)).size();
		    c = clist.get(count);
		}
	    }
	    int index = 0;
	    System.out.println("struct PHRASE_INDEX {");
	    System.out.println("jchar c;");
	    System.out.println("int index;");
	    System.out.println("int size;");
	    System.out.println("int maxphrase;");
	    System.out.println("} phraseindex[] = {");
	    for (int count = 0; count < clist.size(); count++) {
		Character cc = clist.get(count);
		ArrayList<String> sl = map.get(cc);
		
		System.out.println("\t{ " + (int) cc.charValue() + ", " + index + ", " + sl.size() + ", " + maxphrase.get(cc).intValue() + " },");
		index = index + sl.size();
		
		// for (int count0 = 0; count0 < sl.size(); count0++) {
		//     System.out.print("\t{ " + (int) cc.charValue() + ", \"");
		//     System.out.println(sl.get(count0) + "\" }, ");
		// }
	    }
	    System.out.println("};");
	    int maxlen = 0;
	    for (int count = 0; count < clist.size(); count++) {
		Character cc = clist.get(count);
		ArrayList<String> sl = map.get(cc);
		for (int count0 = 0; count0 < sl.size(); count0++) {
		    if (sl.get(count0).length() > maxlen) maxlen = sl.get(count0).length();
		}
	    }

	    System.out.println("jchar phrase[][" + (maxlen + 2) + "] = {");
	    for (int count = 0; count < clist.size(); count++) {
		Character cc = clist.get(count);
		ArrayList<String> sl = map.get(cc);
		for (int count0 = 0; count0 < sl.size(); count0++) {
		    String str = sl.get(count0);
		    // System.out.println("\"" + sl.get(count0) + "\", ");
		    // if (sl.get(count0).length() > maxlen) maxlen = sl.get(count0).length();
		    System.out.print("\t{ ");
		    System.out.print(str.length() + ", ");
		    for (int count1 = 0; count1 < str.length(); count1++) {
			System.out.print((int) str.charAt(count1));
			if (count1 != (str.length() - 1)) System.out.print(", ");
		    }
		    if (str.length() < (maxlen + 1)) {
			for (int count1 = str.length(); count1 < (maxlen + 1); count1++) {
			    System.out.print(", 0");
			}
		    }
		    System.out.println(" }, ");
		}
	    }
	    System.out.println("};");

	    System.out.println("#define MAX_PHRASE_LENGTH " + maxlen);
	    System.err.println("Max Phrase Length : " + maxlen);
	    // System.out.println((int) c + " " + max);
	    total = 0; max = 0;
	    String d = "";
	    for (int count = 0; count < clist.size(); count++) {
		total = total + dword.get(clist.get(count)).size();
		HashMap<String, ArrayList<String>> slist = dword.get(clist.get(count));
		Object[] strset = slist.keySet().toArray();
		for (int count0 = 0; count0 < strset.length; count0++) {
		    if (slist.get(strset[count0]).size() > max) {
			d = strset[count0].toString();
			max = slist.get(strset[count0]).size();
		    }
		}
	    }
	    // System.out.println(total);
	    // System.out.println(d + " " + max);
	    reader.close();
	    fo.close();

	    System.err.println("Max Phrase " + max_phrase);

	} catch (Exception ex) {
	    ex.printStackTrace();
	}
    }

}
