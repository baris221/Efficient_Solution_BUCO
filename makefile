.PHONY: all dp greedy ts  clean

all: dp greedy ts

dp:
	$(MAKE) -C DP_BUCO

greedy:
	$(MAKE) -C FKSS_BUCO

ts:
	$(MAKE) -C TS_BUCO


clean:
	$(MAKE) -C DP_BUCO clean
	$(MAKE) -C FKSS_BUCO clean
	$(MAKE) -C TS_BUCO clean