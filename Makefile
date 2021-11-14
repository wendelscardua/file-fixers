PROJECT=file-fixers
LD65_FLAGS=
CA65_FLAGS=
EMULATOR=/mnt/c/NESDev/Mesen.exe
NSF2DATA=/mnt/c/NESDev/famitone5.0/nsf2data/nsf2data5.exe
TEXT2DATA=/mnt/c/NESDev/famitone5.0/text2data/text2vol5.exe
FAMITRACKER=/mnt/c/NESDev/famitracker/FamiTracker.exe
VERSION := $(shell git describe --exact-match --tags 2> /dev/null || git rev-parse --short HEAD)

TARGET=${PROJECT}.nes

.PHONY: debug run usage release FORCE

default: ${TARGET}

debug: LD65_FLAGS += --dbgfile ${PROJECT}.dbg
debug: CA65_FLAGS += -g -DDEBUG=1
debug: ${TARGET}

${TARGET}: src/main.o src/crt0.o src/lib/unrle.o src/lib/subrand.o \
           src/nametable_loader.o \
           src/dungeon.o \
           src/players.o \
           src/entities.o  \
           assets/nametables.o \
           assets/palettes.o \
           assets/sectors.o \
           assets/sprites.o \
           assets/enemy-stats.o
	ld65 $^ -C MMC3.cfg nes.lib -m map.txt -o ${TARGET} ${LD65_FLAGS}

%.o: %.s
	ca65 $< ${CA65_FLAGS}

src/main.s: src/main.c \
            assets/nametables.h assets/palettes.h \
            assets/sprites.h \
            src/lib/unrle.h src/nametable_loader.h src/dungeon.h
	cc65 -Oirs $< --add-source ${CA65_FLAGS}

src/nametable_loader.s: src/nametable_loader.c
	cc65 -Oirs $< --add-source ${CA65_FLAGS}

src/dungeon.s: src/dungeon.c src/dungeon.h src/entities.h
	cc65 -Oirs $< --add-source ${CA65_FLAGS}

src/entities.s: src/entities.c src/entities.h src/players.h src/directions.h assets/sprites.h
	cc65 -Oirs $< --add-source ${CA65_FLAGS}

src/players.s: src/players.c src/players.h src/charmap.h
	cc65 -Oirs $< --add-source ${CA65_FLAGS}

src/crt0.o: src/crt0.s src/mmc3/mmc3_code.asm src/lib/neslib.s src/lib/nesdoug.s assets/*.chr \
            src/music/soundtrack.s src/music/soundfx.s
	ca65 $< ${CA65_FLAGS}

assets/nametables.o: assets/nametables.s assets/nametables.h \
                     assets/nametables/title.rle \
                     assets/nametables/main-window.rle \
                     assets/nametables/drivers-window.rle \
                     assets/nametables/dungeon-hud.rle \
                     assets/nametables/actions-menu.rle
	ca65 $< ${CA65_FLAGS}

assets/sectors.o: assets/sectors.s assets/sectors.h src/charmap.inc \
                  assets/sectors/sector-00.bin \
                  assets/sectors/sector-00-room.bin \
                  assets/sectors/sector-01.bin \
                  assets/sectors/sector-01-room.bin \
                  assets/sectors/sector-02.bin \
                  assets/sectors/sector-02-room.bin \
                  assets/sectors/sector-03.bin \
                  assets/sectors/sector-03-room.bin \
                  assets/sectors/sector-04.bin \
                  assets/sectors/sector-04-room.bin \
                  assets/sectors/sector-05.bin \
                  assets/sectors/sector-05-room.bin \
                  assets/sectors/sector-06.bin \
                  assets/sectors/sector-06-room.bin \
                  assets/sectors/sector-07.bin \
                  assets/sectors/sector-07-room.bin
	ca65 $< ${CA65_FLAGS}

assets/palettes.o: assets/palettes.s assets/palettes.h \
                   assets/bg.pal assets/sprites.pal \
                   assets/bg-dungeon.pal assets/sprites-dungeon.pal
	ca65 $< ${CA65_FLAGS}

assets/sprites.o: assets/sprites.s assets/sprites.h
	ca65 $< ${CA65_FLAGS}

assets/enemy-stats.o: assets/enemy-stats.s assets/enemy-stats.h
	ca65 $< ${CA65_FLAGS}

assets/enemy-stats.s: assets/enemy-stats.yaml src/enemies.inc tools/compile-enemy-stats.rb
	ruby tools/compile-enemy-stats.rb $< $@

assets/sectors/%.bin: assets/sectors/%.tmx tools/sector-to-bin.rb
	ruby tools/sector-to-bin.rb $< $@

assets/sectors/%-room.bin: assets/sectors/%.tmx tools/sector-to-room-bin.rb
	ruby tools/sector-to-room-bin.rb $< $@

src/music/soundtrack.s: src/music/soundtrack.txt
	${TEXT2DATA} $^ -ca65 -allin

src/music/soundtrack.txt: src/music/soundtrack.ftm
	${FAMITRACKER} $^ -export $@

src/music/soundfx.nsf: src/music/soundfx.ftm
	${FAMITRACKER} src/music/soundfx.ftm -export src/music/soundfx.nsf

src/music/soundfx.s: src/music/soundfx.nsf
	${NSF2DATA} src/music/soundfx.nsf -ca65 -ntsc

%.rle: %.nam
	ruby tools/rle-compress.rb $< $@

assets/nametables/main-window.nam: assets/nametables/main-window.map \
                                   assets/nametables/desktop.nam
	ruby tools/prerender-window.rb assets/nametables/desktop.nam \
                                       $< \
                                       6 6 \
                                       $@

assets/nametables/drivers-window.nam: assets/nametables/drivers-window.map \
                                      assets/nametables/main-window.nam
	ruby tools/prerender-window.rb assets/nametables/main-window.nam \
                                       $< \
                                       4 8 \
                                       $@

clean:
	rm src/*.o src/main.s src/*/*.o \
           src/music/soundfx.nsf src/music/soundfx.s \
           src/music/soundtrack.txt src/music/soundtrack.s \
           assets/*.o assets/nametables/*.rle assets/nametables/*-window.nam \
           *.nes *.dbg map.txt -f

run: debug
	${EMULATOR} ${TARGET}

usage: tools/ld65-map.json

tools/ld65-map.json: map.txt MMC3.cfg tools/ld65-map.rb
	ruby tools/ld65-map.rb map.txt MMC3.cfg tools/ld65-map.json

release: ${TARGET}
	cp ${TARGET} ${PROJECT}-${VERSION}.nes

FORCE:
