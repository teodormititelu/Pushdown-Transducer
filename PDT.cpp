#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include <unordered_map>

using namespace std;

const char lambda = '@';
const string pop = "pop";
const string push = "push";

ifstream fin("PDT.in");

class transition{
    string before_state; // state before transition
    string new_state; // state after transition

    char top_of_the_stack;
    char input_symbol;
    string output_symbol;

    vector < string > stack_actions;

public:
    transition( string s1, char in_s, char top, string s2, string out_s, const vector < string > & actions ){
        before_state = s1;
        new_state = s2;
        top_of_the_stack = top;
        input_symbol = in_s;
        output_symbol = out_s;
        stack_actions = actions;
    }

    bool transition_is_possible( const stack < char >& S ){
        int st_size = S.size();
        for( auto action : stack_actions )
            if( action.compare(pop) == 0 && st_size == 1)
                    return false;
            else st_size++;
        return true;
    }
    void apply_transition( stack < char > & S, string & output ){

        for( auto action : stack_actions ){
            if( action.compare(pop) == 0 )
                S.pop();
            else
                if( action[5] != '@' ) S.push( action[5] );
        }
        if( output_symbol != "@")
            output = output + output_symbol;
    }
    string get_state(){
        return new_state;
    }

    void print(){
        cout << before_state << ' ' << input_symbol << ' ' << top_of_the_stack << ' ' << new_state << ' ' << output_symbol << ' ';
        for( auto action : stack_actions )
            cout << action << ' ';
        cout << '\n';
    }
};

string InitialStates; //initial state
unordered_map < string, bool > FinalStates; //final states
//vector < transition > Transitions; //transitions
unordered_map < string, vector < transition > > Transitions;


void SplitFinalStates(string str)
{
    string state = "";
    char del = ' ';
    for (auto c : str){
        if (c == del){
            FinalStates[state] = true;
            state = "";
        }
        else {
            state = state + c;
        }
    }
    FinalStates[state] = true;
}

void DFS( string state, string input_word, int index, string output_word, stack <char> S){

    //cout << "!!" << state << ' ' << index << ' ' << S.top() << ' ' << S.size() << ' ' << output_word << '\n';
    //if( output_word.size() > 10 ) return;
    if( index == input_word.size() ){
        if( FinalStates.find(state) != FinalStates.end() )
            cout << output_word << '\n';

        char token = '@';

        char top_symbols[] = {S.top(), '@'};

        for( int j = 0; j < 2; ++j ){
            char top = top_symbols[j];

            string configuration = state + token + top;

            if( Transitions.find( configuration ) != Transitions.end() )
                for( auto production : Transitions[configuration] )
                    if( production.transition_is_possible( S ) ){
                        stack < char > rollback_S = S;
                        string rollback_output = output_word;

                        production.apply_transition( S, output_word );
                        if( token != '@')
                            DFS( production.get_state(), input_word, index+1, output_word, S );
                        else DFS( production.get_state(), input_word, index, output_word, S );

                        S = rollback_S;
                        output_word = rollback_output;
                    }
        }
        return;
    }

    char tokens[] = {input_word[index], '@'};
    char top_symbols[] = {S.top(), '@'};

    for( int i = 0; i < 2; ++i)
        for( int j = 0; j < 2; ++j ){
            char token = tokens[i];
            char top = top_symbols[j];

            string configuration = state + token + top;

            if( Transitions.find( configuration ) != Transitions.end() )
                for( auto production : Transitions[configuration] )
                    if( production.transition_is_possible( S ) ){
                        stack < char > rollback_S = S;
                        string rollback_output = output_word;

                        production.apply_transition( S, output_word );
                        if( token != '@')
                            DFS( production.get_state(), input_word, index+1, output_word, S );
                        else DFS( production.get_state(), input_word, index, output_word, S );

                        S = rollback_S;
                        output_word = rollback_output;
                    }
        }
}
void Compute( string input_word ){
    string output_word = "";
    stack < char > S;

    S.push('$');

    cout << "Accepted outputs:\n";
    DFS( InitialStates, input_word, 0, output_word, S );

    cout << "\"" << input_word << "\" done\n";

}
int main()
{
    string finalStates;

    getline(fin, InitialStates);
    getline(fin, finalStates );
    SplitFinalStates( finalStates );

    while( !fin.eof() ){
        string s1, s2;
        char top, in_s;
        string out_s;
        vector <string> actions;
        string actions_string;

        fin >> s1 >> in_s >> top >> s2 >> out_s;

        if( s1.empty() )
            continue;

        getline(fin, actions_string);

        if( !actions_string.empty() ){
            string action = "";
            char del = ',';
            for (auto c : actions_string){
                if( c == ' ' )continue;
                if (c == del){
                    actions.push_back( action );
                    action = "";
                }
                else {
                    action = action + c;
                }
            }
            actions.push_back( action );
        }
        string map_key = s1+in_s+top;
        Transitions[map_key].push_back( transition(s1, in_s, top, s2, out_s, actions) );
        //cout << map_key << ' ' << Transitions[map_key].size() << '\n';
    }

    string input;
    cin >> input;
    while( input.compare("0") ){
        Compute(input);
        cin >> input;
    }

    return 0;
}
