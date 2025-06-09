import xml.etree.ElementTree as ET
import sys
import os

def parse_jff_mealy(filename):
    """
    Parses a JFLAP Mealy machine .jff file and prints transitions
    including whether the destination state is final.

    Args:
        filename (str): The path to the .jff file.

    Returns:
        None. Prints the output to stdout.
    """
    if not os.path.exists(filename):
        print(f"Error: File not found: {filename}", file=sys.stderr) # Print errors to stderr
        return

    try:
        tree = ET.parse(filename)
        root = tree.getroot()

        # Basic check if it looks like a JFLAP Mealy file
        file_type = root.find('type')
        if file_type is None or file_type.text != 'mealy':
            print(f"Warning: File '{filename}' might not be a Mealy machine JFF file (type='{file_type.text if file_type is not None else 'None'}'). Proceeding anyway.", file=sys.stderr)

        automaton = root.find('automaton')
        if automaton is None:
            print(f"Error: <automaton> tag not found in {filename}", file=sys.stderr)
            return

        # pre-header with the number of states and transitions
        number_states = 0
        number_transitions = 0
        for state in automaton.findall('state'):
            number_states += 1

        for transition in automaton.findall('transition'):
            number_transitions += 1

        print(f"{number_states},{number_transitions}")


        # --- Step 1: Identify all final states ---
        final_state_ids = set()
        
        for state in automaton.findall('state'):
            state_id = state.get('id') # Get the 'id' attribute
            if state_id is None:
                print(f"Warning: Skipping state element without an 'id' attribute.", file=sys.stderr)
                continue


            # Check for a <label> sub-element with text 'final'
            label_element = state.find('label')
            if label_element is not None and label_element.text == 'final':
                final_state_ids.add(state_id)
            # Also check for a <final/> tag (alternative JFLAP format)
            final_tag = state.find('final')
            if final_tag is not None:
                 final_state_ids.add(state_id)


        # --- Step 2: Process transitions and print output ---

        # print the principal header
        print("state,symbol,num_outputs,outputs,next_state,next_state_is_final")

        # Find all transition elements and extract data
        for transition in automaton.findall('transition'):
            from_state = transition.find('from')
            to_state = transition.find('to')
            read_symbol = transition.find('read')
            transout_output = transition.find('transout') # output for Mealy

            # Ensure all required elements are present within the transition
            if None in [from_state, to_state, read_symbol, transout_output]:
                print("Warning: Skipping incomplete transition element.", file=sys.stderr)
                continue

            from_id = from_state.text
            to_id = to_state.text
            read_sym = read_symbol.text

            # Handle case where transout might be empty tag e.g. <transout/>
            trans_out = transout_output.text if transout_output.text is not None else ""
            num_output = trans_out.count(',') + 1
            trans_out = trans_out.replace(", ", " ")

            # Ensure essential text data is present (output can be empty)
            if None in [from_id, to_id, read_sym]:
                 print(f"Warning: Skipping transition with missing text data: from={from_id}, to={to_id}, read={read_sym}, output={trans_out}", file=sys.stderr)
                 continue

            # Determine if the destination state ('to_state') is final
            is_final_value = 1 if to_id in final_state_ids else 0

            # Format and print the output line
            print(f"{from_id},{read_sym},{num_output},{trans_out},{to_id},{is_final_value}")

    except ET.ParseError as e:
        print(f"Error parsing XML file {filename}: {e}", file=sys.stderr)
    except Exception as e:
        print(f"An unexpected error occurred: {e}", file=sys.stderr)

# --- Main execution block ---
if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python parse_jff.py <filename.jff>", file=sys.stderr)
        sys.exit(1) # Exit with an error code

    jff_file = sys.argv[1]
    parse_jff_mealy(jff_file)
