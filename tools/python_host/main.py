from pygments.lexers.scripting import LuaLexer
from prompt_toolkit import PromptSession
from prompt_toolkit.history import FileHistory
from prompt_toolkit.lexers import PygmentsLexer

from glowie import find_glowie

def main():
    prompt_session = PromptSession(
        history=FileHistory(".glw_history"),
        multiline=True,
        message="> ",
        prompt_continuation="+ ",
        lexer=PygmentsLexer(LuaLexer),
    )

    print("Looking for glowies in the wild...")

    glowie = find_glowie()
    if glowie is None:
        print("No glowies found :(")
        return

    print("Found a glowie! Connecting...")

    with glowie:
        print("Connected\n")
        print("Press Esc + Enter to send script")
        while True:
            try:
                cmd = prompt_session.prompt()
                glowie.send_command(cmd)
            except KeyboardInterrupt:
                break


if __name__ == "__main__":
    main()
