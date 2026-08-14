import pygame
import pexpect
import re
import threading
import queue
import sys

# ============================================================
# BLE CONFIG
# ============================================================

MAC = "73:E4:80:4A:E0:A7"

NOTIFY_HANDLE = "0x0010"
WRITE_HANDLE = "0x0013"

# ============================================================
# PYGAME CONFIG
# ============================================================

WIDTH = 1000
HEIGHT = 650

BG = (18, 18, 22)
PANEL_BG = (28, 28, 34)
INPUT_BG = (38, 38, 46)

TEXT = (230, 230, 230)
RX_COLOR = (100, 220, 130)
TX_COLOR = (100, 170, 255)
DIM_TEXT = (150, 150, 160)

BORDER = (70, 70, 80)
BUTTON_BG = (55, 55, 65)
BUTTON_HOVER = (75, 75, 90)

FONT_SIZE = 21
TITLE_SIZE = 24

TITLE_HEIGHT = 55
INPUT_HEIGHT = 55
BOTTOM_MARGIN = 12
PANEL_GAP = 10

CLEAR_BUTTON_WIDTH = 90

# ============================================================
# GLOBALS
# ============================================================

rx_queue = queue.Queue()

# Stores:
# ("RX", "message")
# ("TX", "message")
messages = []

# ============================================================
# GATTTOOL
# ============================================================

print("Starting gatttool...")

child = pexpect.spawn(
    f"gatttool -b {MAC} -I",
    timeout=None,
    encoding="utf-8"
)

print("Connecting...")

child.sendline("connect")

child.expect(
    "Connection successful",
    timeout=20
)

print("BLE connected!")


# ============================================================
# RECEIVE THREAD
# ============================================================

notification_pattern = re.compile(
    r"Notification handle = 0x[0-9a-fA-F]+ value: ([0-9a-fA-F ]+)"
)


def receive_thread():

    while True:

        try:
            child.expect(r"\r\n", timeout=None)

            line = child.before.strip()

            match = notification_pattern.search(line)

            if not match:
                continue

            hex_string = match.group(1)

            try:

                data = bytes.fromhex(
                    hex_string.replace(" ", "")
                )

                message = data.decode(
                    errors="replace"
                )

                rx_queue.put(message)

            except ValueError:
                pass

        except Exception as e:

            rx_queue.put(
                f"[BLE ERROR] {e}"
            )

            break


rx = threading.Thread(
    target=receive_thread,
    daemon=True
)

rx.start()


# ============================================================
# SEND
# ============================================================

def send_message(message):

    if not message:
        return

    try:

        data = (message + '\n').encode()

        hex_data = data.hex()

        command = (
            f"char-write-req "
            f"{WRITE_HANDLE} "
            f"{hex_data}"
        )

        child.sendline(command)

        # Show sent message immediately
        messages.append(
            ("TX", message)
        )

    except Exception as e:

        messages.append(
            ("ERROR", f"[SEND ERROR] {e}")
        )


# ============================================================
# PYGAME
# ============================================================

pygame.init()

screen = pygame.display.set_mode(
    (WIDTH, HEIGHT)
)

pygame.display.set_caption(
    "STM32 BLE Terminal"
)

font = pygame.font.Font(
    None,
    FONT_SIZE
)

title_font = pygame.font.Font(
    None,
    TITLE_SIZE
)

clock = pygame.time.Clock()

pygame.key.start_text_input()


# ============================================================
# STATE
# ============================================================

running = True

input_text = ""

scroll_offset = 0

clear_rect = pygame.Rect(
    WIDTH - CLEAR_BUTTON_WIDTH - 15,
    10,
    CLEAR_BUTTON_WIDTH,
    34
)


# ============================================================
# MAIN LOOP
# ============================================================

while running:

    # --------------------------------------------------------
    # EVENTS
    # --------------------------------------------------------

    for event in pygame.event.get():

        if event.type == pygame.QUIT:

            running = False

        elif event.type == pygame.TEXTINPUT:

            input_text += event.text

        elif event.type == pygame.KEYDOWN:

            if event.key == pygame.K_RETURN:

                if input_text.strip():

                    send_message(
                        input_text
                    )

                    input_text = ""

            elif event.key == pygame.K_BACKSPACE:

                input_text = input_text[:-1]

            elif event.key == pygame.K_ESCAPE:

                running = False

        elif event.type == pygame.MOUSEBUTTONDOWN:

            if event.button == 1:

                if clear_rect.collidepoint(
                    event.pos
                ):

                    messages.clear()
                    scroll_offset = 0

    # --------------------------------------------------------
    # RECEIVE MESSAGES
    # --------------------------------------------------------

    new_message = False

    while not rx_queue.empty():

        message = rx_queue.get()

        # Handle messages containing multiple lines
        lines = message.splitlines()

        if not lines:

            lines = [message]

        for line in lines:

            messages.append(
                ("RX", line)
            )

            new_message = True

    # --------------------------------------------------------
    # LAYOUT
    # --------------------------------------------------------

    message_top = TITLE_HEIGHT

    message_bottom = (
        HEIGHT
        - INPUT_HEIGHT
        - BOTTOM_MARGIN
        - PANEL_GAP
    )

    message_height = (
        message_bottom
        - message_top
    )

    # --------------------------------------------------------
    # DRAW BACKGROUND
    # --------------------------------------------------------

    screen.fill(BG)

    # --------------------------------------------------------
    # TITLE
    # --------------------------------------------------------

    title = title_font.render(
        "STM32 BLE TERMINAL",
        True,
        TEXT
    )

    screen.blit(
        title,
        (18, 14)
    )

    # --------------------------------------------------------
    # CLEAR BUTTON
    # --------------------------------------------------------

    mouse_pos = pygame.mouse.get_pos()

    if clear_rect.collidepoint(mouse_pos):

        button_color = BUTTON_HOVER

    else:

        button_color = BUTTON_BG

    pygame.draw.rect(
        screen,
        button_color,
        clear_rect,
        border_radius=6
    )

    clear_text = font.render(
        "CLEAR",
        True,
        TEXT
    )

    clear_text_rect = clear_text.get_rect(
        center=clear_rect.center
    )

    screen.blit(
        clear_text,
        clear_text_rect
    )

    # --------------------------------------------------------
    # MESSAGE PANEL
    # --------------------------------------------------------

    message_rect = pygame.Rect(
        10,
        message_top,
        WIDTH - 20,
        message_height
    )

    pygame.draw.rect(
        screen,
        PANEL_BG,
        message_rect,
        border_radius=6
    )

    pygame.draw.rect(
        screen,
        BORDER,
        message_rect,
        width=1,
        border_radius=6
    )

    # --------------------------------------------------------
    # MESSAGE RENDERING
    # --------------------------------------------------------

    line_height = FONT_SIZE + 8

    available_lines = (
        message_height - 16
    ) // line_height

    visible_messages = messages[
        -available_lines:
    ]

    y = message_top + 10

    for direction, message in visible_messages:

        if direction == "RX":

            prefix = "STM32: "
            color = RX_COLOR

        elif direction == "TX":

            prefix = "TX: "
            color = TX_COLOR

        else:

            prefix = ""
            color = TEXT

        prefix_surface = font.render(
            prefix,
            True,
            color
        )

        message_surface = font.render(
            message,
            True,
            TEXT
        )

        screen.blit(
            prefix_surface,
            (22, y)
        )

        screen.blit(
            message_surface,
            (
                22 + prefix_surface.get_width(),
                y
            )
        )

        y += line_height

    # --------------------------------------------------------
    # INPUT PANEL
    # --------------------------------------------------------

    input_y = (
        HEIGHT
        - INPUT_HEIGHT
        - BOTTOM_MARGIN
    )

    input_rect = pygame.Rect(
        10,
        input_y,
        WIDTH - 20,
        INPUT_HEIGHT
    )

    pygame.draw.rect(
        screen,
        INPUT_BG,
        input_rect,
        border_radius=6
    )

    pygame.draw.rect(
        screen,
        BORDER,
        input_rect,
        width=1,
        border_radius=6
    )

    # Input text

    prompt_surface = font.render(
        "> " + input_text,
        True,
        TEXT
    )

    screen.blit(
        prompt_surface,
        (22, input_y + 15)
    )

    # --------------------------------------------------------
    # DISPLAY
    # --------------------------------------------------------

    pygame.display.flip()

    clock.tick(60)


# ============================================================
# CLEANUP
# ============================================================

pygame.key.stop_text_input()

try:

    child.sendline("disconnect")
    child.close()

except Exception:
    pass

pygame.quit()

sys.exit()
