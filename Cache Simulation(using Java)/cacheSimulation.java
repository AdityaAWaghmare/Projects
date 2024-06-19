import java.util.Scanner;
import java.util.ArrayList;
import java.io.File;
import java.io.FileNotFoundException;

class Set {
	int hits = 0, misses = 0;
	ArrayList<String> way = new ArrayList<>();

	public void coreRequest(String binAddress, int tagEndBit, int Associativity) {
		boolean ifHit = false;
		int index = -1;
		String tag = binAddress.substring(0, tagEndBit);
		// checking for a hit
		for (String element : way) {
			index++;
			if ((element.substring(0, tagEndBit)).equals(tag)) {
				ifHit = true;
				break;
			}

		}

		if (ifHit) {
			hits++;

			// removed to change index to most recently used
			way.remove(index);
		} else {
			misses++;

			if (way.size() == Associativity) {
				way.remove(0);
			}
		}
		// as most recently used or new
		way.add(binAddress);

	}
}

public class cacheSimulation {

	public static int logBase2(int number) {
		int ans = 0;
		while (number > 1) {
			number /= 2;
			ans++;
		}
		return ans;
	}

	public static String hexToBin(String inputString) {
		int hexDigitInDecimal;
		String binAddress = "";
		String hexDigitInBinary;
		for (int i = 0; i < inputString.length(); i++) {
			//getting decimal for every hex char
			hexDigitInDecimal = Integer.parseInt(Character.toString(inputString.charAt(i)), 16);
			//convering it to binary
			hexDigitInBinary = Integer.toBinaryString(hexDigitInDecimal);
			//converting it to 4 digit binary
			hexDigitInBinary = String.format("%4s", hexDigitInBinary).replace(' ', '0');
			//concatinating string
			binAddress = binAddress.concat(hexDigitInBinary);
		}
		binAddress = String.format("%32s", binAddress).replace(' ', '0');
		return binAddress;
	}

	public static int setIndexBits, offsetBits, totalHits = 0, totalMisses = 0;

	public static void main(String[] args) {

		System.out.println("Cache Simulation --start--");

		double cacheSizeKB = Double.parseDouble(args[0]);

		int Associativity = Integer.parseInt(args[1]);

		String traceFilePath = args[3];

		int blockSize = Integer.parseInt(args[2]);

		int setIndexBits, offsetBits, totalHits = 0, totalMisses = 0;

		// calc total sets
		int totalSets = (int)( cacheSizeKB * 1024 / (Associativity * blockSize) );

		// calc no. of bits of set index and offset
		setIndexBits = logBase2(totalSets);
		offsetBits = logBase2(blockSize);

		// making set object array
		Set[] aSet = new Set[totalSets];

		try {
			// for reading file
			File file = new File(traceFilePath);
			// file scanner object
			Scanner fileInput = new Scanner(file);
			// String stringInput;
			String binAddress;
			// reading file for addresses and giving core request
			while (fileInput.hasNext()) {
				// taking one binary address at a time
				binAddress = hexToBin(fileInput.next());

				// calc set index for that address
				int setIndex = Integer.parseInt(binAddress.substring((32 - offsetBits - setIndexBits), (32 - offsetBits)), 2);

				// instantiating null elements
				if (aSet[setIndex] == null) {
					aSet[setIndex] = new Set();
				}

				// giving core request
				aSet[setIndex].coreRequest(binAddress, (32 - offsetBits - setIndexBits), Associativity);
			}
			// freeing resouces for file scanner
			fileInput.close();
		} catch (FileNotFoundException e) {
			System.out.println("\nFile not found: " + e.getMessage());
		}
		// for space
		System.out.println("");

		for (int i = 0; i < totalSets; i++) {
			if (aSet[i] != null) {
				System.out.println("Set[" + (i + 1) + "] : Hits = " + aSet[i].hits + " , Misses = " + aSet[i].misses + "\n");
				totalHits += aSet[i].hits;
				totalMisses += aSet[i].misses;
			}
			else{
				System.out.println("Set[" + (i + 1) + "] : Hits = " + 0 + " , Misses = " + 0 + "\n");	
			}
		}

		System.out.println("Total Hits = " + totalHits);
		System.out.println("Total Misses = " + totalMisses);

		System.out.println("\n\nCache Simulation --end--");
	}
}
