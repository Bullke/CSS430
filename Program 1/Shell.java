////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// Nenad Bulicic                                                                           \\
// CSS 430 - Program 1: System Calls and Shell                                             \\
////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// This assignment intends (part 1) to familiarize you with Linux programming using        \\
// several system calls such as fork, execlp, wait, pipe, dup2, and close, and (part 2) to \\
// help you understand that, from the kernel's view point, the shell is simply viewed as   \\
// an application program that uses system calls to spawn and to terminate other user	   \\
// programs. Also to become familiar with the ThreadOS operating system simulator in       \\
// part 2 of this assignment.                     										   \\
////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// 					  			PART 2: Shell and ThreadOS                                 \\
//																						   \\
// This implementation is a Java program, named Shell.java that will be invoked from       \\
// ThreadOS Loader and behave as a shell command interpreter. Once invoked, Shell.java     \\
// prints a command prompt:																   \\
// shell[1]%																			   \\
// The user input commands, delimited by '&' or ';' and Shell.java executes them as an     \\
// independent child thread with a SysLib.exec( ) system call. Symbol '&' means a          \\
// concurrent execution, while the symbol ';' means a sequential execution. Thus, when     \\
// encountering a delimiter ';', Shell.java calls SysLib.join( ) system call(s) in order   \\
// to wait for this child thread to be terminated. Since SysLib.join( ) may return the ID  \\
// of any child thread that has been previously terminated, this calling SysLib.join( )    \\
// process is repeated until it returns the exact ID of the child thread needed            \\
////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

import java.util.HashSet;  // For keeping track of child joining

public class Shell extends Thread
{
	public Shell()
	{}
	protected String userName = "Shell";
	// Custom username to display instead of Shell
	public Shell(String[] args)
	{
		userName = args[1];
	}
	public void run()
	{
		int lineNumber = 1;
		// Keep taking user commands until it is "exit"
		while(true)
		{
			SysLib.cout( userName + "[" + lineNumber + "]% " );
			StringBuffer inputBuffer = new StringBuffer();
			SysLib.cin(inputBuffer);
			String stringCommands = new String(inputBuffer);
			// Always check first for exit then execute commands
			if(stringCommands.compareTo("exit") == 0)
			{
				SysLib.cout("exit\n");
				SysLib.exit();
				break;
			}
			if(!stringCommands.isEmpty())
			{
				// Separates concurrent commands from sequential
				for(String inputCommands : stringCommands.split(";"))
				{
					executeCommands(inputCommands);
				}
			}
			lineNumber++;
		}
	}
	// executeCoommands executes concurent commands one by one delimiting
	// by '&', since the parameter is a String delimited by ';'.
	// executeCommands must wait for all child processes to join before finishing
	public void executeCommands(String inputString)
	{
		int childID;
		// For child process ID storage
		HashSet<Integer> childThreadIDs = new HashSet<Integer>();
		for(String inputCommands : inputString.split("&"))
		{
			String[] commandString = SysLib.stringToArgs(inputCommands);
			if(commandString.length != 0)
			{
				// Create child process and add to hashset
				if((childID = SysLib.exec(commandString)) > -1)
				{
					SysLib.cout(commandString[0] + "\n");
					childThreadIDs.add(childID);
				}
			}
		}
		// Wait for all child processes to complete execution (or fail) and join
		while(!childThreadIDs.isEmpty())
		{
			if(((childID = SysLib.join()) > -1) && (childThreadIDs.contains(childID)))
			{
				childThreadIDs.remove(childID);
			}
			else
			{
				SysLib.cerr("child join failed\n");
			}
		}
	}
}