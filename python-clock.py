from datetime import datetime
from time import gmtime


def isPrime(n):
    if n <= 3:
        return n > 1
    elif n % 2 == 0 or n % 3 == 0:
        return False
    limit = int(n**0.5) + 1
    for i in range(5, limit, 6):
        if n % i == 0 or n % (i + 2) == 0:
            return False
    return True


def main() -> None:
    lastSec: int = gmtime().tm_sec
    isNextPrime: bool = False
    while True:
        currTime = gmtime()
        currSec: int = currTime.tm_sec
        if currSec != lastSec:
            lastSec = currSec
            secsIntoYear: int = (
                currSec
                + (currTime.tm_min * 60)
                + (currTime.tm_hour * 3600)
                + (currTime.tm_yday * 86400)
            )
            if isNextPrime:
                print(f"{secsIntoYear} (PRIME)")
            else:
                print(secsIntoYear)
            isNextPrime = isPrime(secsIntoYear + 1)


# Uses UNIX timestamp (seconds since midnight 1970-01-01 in UTC)
def unix_main() -> None:
    lastTime: int = int(datetime.now().timestamp())
    isNextPrime: bool = False
    while True:
        currTime = int(datetime.now().timestamp())
        if currTime != lastTime:
            lastTime = currTime
            if isNextPrime:
                print(f"{currTime} (PRIME)")
            else:
                print(currTime)
            isNextPrime = isPrime(currTime + 1)


if __name__ == "__main__":
    # main()
    unix_main()
