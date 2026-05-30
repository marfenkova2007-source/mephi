from collections.abc import Callable, Hashable, Iterable
from collections import OrderedDict, namedtuple
from functools import wraps

def lru_cache(max_items: int) -> Callable:
    """
    Функция создает декоратор, позволяющий кэшировать результаты выполнения обернутой функции по принципу LRU-кэша.
    Размер LRU кэша ограничен количеством max_items. При попытке сохранить новый результат в кэш, в том случае, когда
    размер кэша уже равен max_size, происходит удаление одного из старых элементов, сохраненных в кэше.
    Удаляется тот элемент, к которому обращались давнее всего.
    Также у обернутой функции должен появиться атрибут stats, в котором лежит объект с атрибутами cache_hits и
    cache_misses, подсчитывающие количество успешных и неуспешных использований кэша.
    :param max_items: максимальный размер кэша.
    :return: декоратор, добавляющий LRU-кэширование для обернутой функции.
    """
    class stats:
        def __init__(self):
            self.cache_hits = 0
            self.cache_misses = 0

    def decorator(f):
        s = stats()
        slov = OrderedDict()
        @wraps(f)
        def wrapper(*args, **kwargs):
            key = (hash(str(args)), hash(str(kwargs)))
            if key in slov:
                s.cache_hits+=1
                slov.move_to_end(key, last = True)
                return slov[key]
            s.cache_misses+=1
            res = f(*args, **kwargs)
            slov[key] = res
            while len(slov)>max_items:
                slov.popitem(last = False)
            return res

        wrapper.stats = s
        return wrapper

    return decorator
