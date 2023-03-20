import os


SPACES_IN_TAB = 2
TAB = " " * SPACES_IN_TAB


def open_rcc(nesting_lvl: int = 0) -> str:
    return f'{nesting_lvl * TAB}<RCC>\n'


def close_rcc(nesting_lvl: int = 0) -> str:
    return f'{nesting_lvl * TAB}</RCC>\n'


def get_filename_without_extension(file: str) -> str:
    return os.path.splitext(file)[0]


def comment_tag(comment: str, nesting_lvl: int = 2) -> str:
    return f'{nesting_lvl * TAB}<!-- {comment} -->\n'


def file_tag(alias: str, filepath: str, nesting_lvl: int = 2 ) -> str:
    return f'{nesting_lvl * TAB}<file alias="{alias}">{filepath}</file>\n'


def open_qresourse(prefix: str, nesting_lvl: int = 1) -> str:
    return f'{nesting_lvl * TAB}<qresource prefix="{prefix}">\n'


def close_qresourse(newlines_after_tag: int = 2, nesting_lvl: int = 1) -> str:
    newlines = newlines_after_tag * '\n'
    return f'{nesting_lvl * TAB}</qresource>{newlines}'


if __name__ == '__main__':
    QRC_FILE    = './resources.qrc'
    BG_DIR      = './images/backgrounds'
    CARDS_DIR   = './images/cards'
    SOUND_DIR   = './sound'

    with open(QRC_FILE, 'w') as resources:
        resources.write(open_rcc())

        resources.write(open_qresourse('/background'))
        for bg_file in os.listdir(BG_DIR):
            alias = get_filename_without_extension(bg_file)
            path_to_file = f'{BG_DIR}/{bg_file}'
            resources.write(file_tag(alias, path_to_file))
        resources.write(close_qresourse())

        resources.write(open_qresourse('/card'))
        for suit in os.listdir(CARDS_DIR):
            if suit == 'unknown.png':
                alias = get_filename_without_extension(suit) 
                path_to_file = f'{CARDS_DIR}/{suit}'
                resources.write(comment_tag(alias))
                resources.write(file_tag(alias, path_to_file))
                continue

            resources.write(comment_tag(suit))

            for card in os.listdir(f'{CARDS_DIR}/{suit}'):
                alias = get_filename_without_extension(f'{suit}/{card}') 
                path_to_file = f'{CARDS_DIR}/{suit}/{card}'
                resources.write(file_tag(alias, path_to_file))
        resources.write(close_qresourse())

        resources.write(open_qresourse('/sound'))
        for sound in os.listdir(SOUND_DIR):
            alias = get_filename_without_extension(sound)
            path_to_file = f'{SOUND_DIR}/{sound}'
            resources.write(file_tag(alias, path_to_file))
        resources.write(close_qresourse(1))

        resources.write(close_rcc())