% This LilyPond file was generated by Rosegarden 22.06
\include "nederlands.ly"
\version "2.12.0"
#(set-global-staff-size 18)
#(set-default-paper-size "a4")
global = { 
    \time 4/4
    \skip 8 \skip 1*98 \skip 16 
}
globalTempo = {
    \override Score.MetronomeMark #'transparent = ##t
    \tempo 4 = 90  
}
\score {
    << % common
        % Force offset of colliding notes in chords:
        \override Score.NoteColumn #'force-hshift = #1.0
        % Allow fingerings inside the staff (configured from export options):
        \override Score.Fingering #'staff-padding = #'()

        \context Staff = "track 1, Cello" << 
            \set Staff.instrumentName = \markup { \center-column { "Cello " } }
            \set Staff.midiInstrument = "Cello"
            \set Score.skipBars = ##t
            \set Staff.printKeyCancellation = ##f
            \new Voice \global
            \new Voice \globalTempo
            \set Staff.autoBeaming = ##f % turns off all autobeaming

            \context Voice = "voice 1" {
                % Segment: Cello
                \override Voice.TextScript #'padding = #2.0
                \override MultiMeasureRest #'expand-limit = 1
                \partial 64*8
                \once \override Staff.TimeSignature #'style = #'default 
                \time 4/4
                
%% 0
                \clef "bass"
                a 8  |
                a, 8 [ a c' b 16 a ] b 8 [ e ] e, 16 r b 8  |
                e, 8 [ b d' c' 16 b ] c' 8 [ a ] a, 16 r a 8  |
                c' 16 [ b ] a r c' [ b ] a r e' 8 [ aes ] e 16 r b 8  |
                c' 16 [ b ] a r c' [ b ] a r e 8. r16 r8 e'  |
%% 5
                cis' 8 [ g a, cis' ] d' [ f, d, d' ]  |
                b 8 [ f g, b ] c' [ e, c, a ]  |
                c' 16 [ b ] a 8 [ c' 16 b ] a 8 [ aes 16 fis ] e 8 [ aes 16 fis ] e 8  |
                c' 16 [ b ] a 8 [ c' 16 b ] a 8 [ aes 16 fis ] e 8 [ aes 16 fis ] e 8  |
                f 16 [ e ] d 8 [ f 16 e ] d 8 [ c e a ] c'  |
%% 10
                b 8 [ a 16 aes ] a r d' r c' 8. r16 < c' b > [ < c' b > ] 
                % warning: bar too short, padding with rests
                % 34560 + 3360 < 38400  &&  7/8 < 4/4
                r8  |
                a 8. r16 r8 a [ a, a ] c' [ b 16 a ]  |
                b 8 [ e ] e, 16 r b 8 [ e, b d' ] c' 16 [ b ]  |
                c' 8 [ a ] a, 16 r a 8 [ g 16 b ] d' 8 [ f 16 b ] d' 8  |
                e 8 c' 8. r16 a 8 [ g 16 b ] d' 8 [ f 16 b ] d' 8  |
%% 15
                e 8. r16 r8 g [ a 16 c' b a ] g 8 [ f ]  |
                b 16 [ d' c' b ] a 8 [ g ] c' 16 [ e' d' c' ] b 8 [ a ]  |
                d' 16 [ f' e' d' ] c' 8 [ b ] e' 16 [ g' f' e' ] d' 8 [ c' ]  |
                f 4 r8 d' [ e' d' 16 c' ] d' 8 [ c' 16 b ]  |
                e' 8 [ d' 16 c' ] d' 8 [ c' 16 b ] b 8 c' 8. r16 a 8  |
%% 20
                a, 8 [ a c' b 16 a ] b 8 [ e ] e, 16 r b 8  |
                e, 8 [ b d' c' 16 b ] c' 8 [ a ] a, 16 r a 8  |
                c' 16 [ b ] a r c' [ b ] a r e' 8 [ aes ] e 16 r b 8  |
                c' 16 [ b ] a r c' [ b ] a r e 8. r16 r8 e'  |
                cis' 8 [ g a, cis' ] d' [ f, d, d' ]  |
%% 25
                b 8 [ f g, b ] c' [ e, c, a ]  |
                c' 16 [ b ] a 8 [ c' 16 b ] a 8 [ aes 16 fis ] e 8 [ aes 16 fis ] e 8  |
                c' 16 [ b ] a 8 [ c' 16 b ] a 8 [ aes 16 fis ] e 8 [ aes 16 fis ] e 8  |
                f 16 [ e ] d 8 [ f 16 e ] d 8 [ c e a ] c'  |
                b 8 [ a 16 aes ] a r d' r c' 8. r16 < c' b > [ < c' b > ] 
                % warning: bar too short, padding with rests
                % 107520 + 3360 < 111360  &&  7/8 < 4/4
                r8  |
%% 30
                a 8. r16 r8 a [ a, a ] c' [ b 16 a ]  |
                b 8 [ e ] e, 16 r b 8 [ e, b d' ] c' 16 [ b ]  |
                c' 8 [ a ] a, 16 r a 8 [ g 16 b ] d' 8 [ f 16 b ] d' 8  |
                e 8 c' 8. r16 a 8 [ g 16 b ] d' 8 [ f 16 b ] d' 8  |
                e 8. r16 r8 g [ a 16 c' b a ] g 8 [ f ]  |
%% 35
                b 16 [ d' c' b ] a 8 [ g ] c' 16 [ e' d' c' ] b 8 [ a ]  |
                d' 16 [ f' e' d' ] c' 8 [ b ] e' 16 [ g' f' e' ] d' 8 [ c' ]  |
                f 4 r8 d' [ e' d' 16 c' ] d' 8 [ c' 16 b ]  |
                e' 8 [ d' 16 c' ] d' 8 [ c' 16 b ] b 8 c' 8. r16 c' 8  |
                c 8 [ c' e' d' 16 c' ] d' 8 [ d ] g, 16 r d' 8  |
%% 40
                g, 8 [ d' f' e' 16 d' ] e' 8 [ c' ] c 16 r c' 8  |
                e' 16 [ d' ] c' r g' [ f' ] e' r d' 8 g 16 r r8 g  |
                c' 8 [ e' c' bes ] a [ c' bes a ]  |
                d' 8 [ f' d' c' ] b [ d' c' b ]  |
                e' 8 [ g' e' d' ] c' [ e' d' c' ]  |
%% 45
                f' 8 [ a' f' e' ] < f' e' > 16 [ < f' e' > ] ees' 8 r b  |
                c' 16 b 8 a 16 ees' cis' 8 b 16 e' ees' 8 cis' 16 fis' e' 8 ees' 16  |
                g' 16 [ e' ees' e' ] g' [ e' ees' e' ] b [ e' ees' e' ] g' [ e' ees' e' ]  |
                a 4 r16 c' [ b a ] ees' 4 r16 fis' [ e' ees' ]  |
                fis' 4 r16 a' [ g' fis' ] g' 8 [ fis' 16 e' ] fis' 8 [ ees' ]  |
%% 50
                e' 8. r16 r8 a [ a, a ] c' [ b 16 a ]  |
                b 8 [ e ] e, 16 r b 8 [ e, b d' ] c' 16 [ b ]  |
                c' 8 [ a ] a, 16 r a 8 [ c' 16 b ] a r c' [ b ] a r  |
                e' 8 [ aes ] e 16 r b 8 [ c' 16 b ] a r e' [ d' ] c' r  |
                b 8. r16 r8 b [ c' e' 16 d' ] c' [ b a g ]  |
%% 55
                f 8 [ a d' f' ] b [ d' 16 c' ] b [ a g f ]  |
                e 8 [ g c' e' ] a [ c' 16 b ] a [ g f e ]  |
                d 8 [ f b d' ] aes [ e ] r b  |
                c' 16 [ e' ] aes 8. r16 a 8 [ c' 16 e' ] aes 8. r16 a 8  |
                d' 8 [ c' 16 d' ] e' [ d' c' b ] c' 8 [ b 16 c' ] d' [ c' b a ]  |
%% 60
                e 8 [ aes b d' ] c' [ b 16 a ] b 8 [ a 16 aes ]  |
                a 8 [ f e d ] b [ aes fis e ]  |
                c' 8 [ a g fis ] d' [ b a aes ]  |
                e' 8 [ c' b a ] d' 16 [ cis' ] d' 8 [ d' 16 cis' ] d' 8  |
                f' 8 [ d' cis' d' ] d' 16 [ cis' ] d' 8 [ d' 16 cis' ] d' 8  |
%% 65
                e 16 [ aes fis a ] aes [ b a c' ] b [ d' c' e' ] d' [ fis' e' d' ]  |
                c' 8 [ b 16 a ] b 8 [ aes a, a ] c' [ b 16 a ]  |
                e, 8 [ b d' c' 16 b ] c' 8 [ a a, a ]  |
                d, 8 [ a e, aes ] a, 4.. r16  |
                c' 8 [ c c' e' ] d' 16 [ c' ] d' 8 [ d ] g, 16 r  |
%% 70
                d' 8 [ g, d' f' ] e' 16 [ d' ] e' 8 [ c' ] c 16 r  |
                c' 8 [ e' 16 d' ] c' r g' [ f' ] e' r d' 8 g 16 r r8  |
                g 8 [ c' e' c' ] bes [ a c' bes ]  |
                a 8 [ d' f' d' ] c' [ b d' c' ]  |
                b 8 [ e' g' e' ] d' [ c' e' d' ]  |
%% 75
                c' 8 [ f' a' f' ] e' [ < f' e' > 16 < f' e' > ] ees' 8 r  |
                b 8 c' 16 b 8 a 16 ees' cis' 8 b 16 e' ees' 8 cis' 16 fis' e' _~  |
                e' 16 ees' [ g' e' ees' ] e' [ g' e' ees' ] e' [ b e' ees' ] e' [ g' ] e'  |
                ees' 16 [ e' ] a 4 r16 c' [ b a ] ees' 4 r16 fis'  |
                e' 16 [ ees' ] fis' 4 r16 a' [ g' fis' ] g' 8 [ fis' 16 e' ] fis' 8  |
%% 80
                ees' 8 e' 8. r16 r8 a [ a, a ] c'  |
                b 16 [ a ] b 8 [ e ] e, 16 r b 8 [ e, b d' ]  |
                c' 16 [ b ] c' 8 [ a ] a, 16 r a 8 [ c' 16 b ] a r c' [ b ]  |
                a 16 r e' 8 [ aes ] e 16 r b 8 [ c' 16 b ] a r e' [ d' ]  |
                c' 16 r b 8. r16 r8 b [ c' e' 16 d' ] c' [ b ]  |
%% 85
                a 16 [ g ] f 8 [ a d' f' ] b [ d' 16 c' ] b [ a ]  |
                g 16 [ f ] e 8 [ g c' e' ] a [ c' 16 b ] a [ g ]  |
                f 16 [ e ] d 8 [ f b d' ] aes [ e ] r  |
                b 8 [ c' 16 e' ] aes 8. r16 a 8 [ c' 16 e' ] aes 8. r16  |
                a 8 [ d' ] c' 16 [ d' e' d' ] c' [ b ] c' 8 [ b 16 c' ] d' [ c' ]  |
%% 90
                b 16 [ a ] e 8 [ aes b d' ] c' [ b 16 a ] b 8  |
                a 16 [ aes ] a 8 [ f e d ] b [ aes ] fis  |
                e 8 [ c' a g ] fis [ d' b a ]  |
                aes 8 [ e' c' b ] a [ d' 16 cis' ] d' 8 [ d' 16 cis' ]  |
                d' 8 [ f' d' cis' ] d' [ d' 16 cis' ] d' 8 [ d' 16 cis' ]  |
%% 95
                d' 8 [ e 16 aes ] fis [ a aes b ] a [ c' b d' ] c' [ e' d' fis' ]  |
                e' 16 [ d' ] c' 8 [ b 16 a ] b 8 [ aes a, a ] c'  |
                b 16 [ a ] e, 8 [ b d' c' 16 b ] c' 8 [ a ] a,  |
                a 8 [ d, a e, ] aes a, 4. _~  |
                a, 16  |
                \bar "|."
            } % Voice
        >> % Staff ends
        \context GrandStaff = "1" <<

            \context Staff = "track 2, Piano" << 
                \set Staff.instrumentName = \markup { \center-column { "Piano " } }
                \set Staff.midiInstrument = "Acoustic Grand Piano"
                \set Score.skipBars = ##t
                \set Staff.printKeyCancellation = ##f
                \new Voice \global
                \new Voice \globalTempo
                \set Staff.autoBeaming = ##f % turns off all autobeaming

                \context Voice = "voice 2" {
                    % Segment: Acoustic Grand Piano
                    \override Voice.TextScript #'padding = #2.0
                    \override MultiMeasureRest #'expand-limit = 1
                    \partial 64*8
                    \once \override Staff.TimeSignature #'style = #'default 
                    \time 4/4
                    
%% 0
                    \clef "treble"
                    r8  |
                    < c' e' a' > 8. r16 r8 a' < aes' b' e'' > r r4  |
                    < aes b e' > 8. r16 r8 e' < c' e' a' > r r4  |
                    r8 a' 16 r e'' r e' r < e' b' > 8 r r4  |
                    r8 e' 16 r c'' r e' r < e' aes' > 8. r16 r4  |
%% 5
                    < a' cis'' e'' > 4 r8 e'' 16 [ g'' ] < a' d'' f'' > 4 r  |
                    < g' b' d'' > 4 r8 d'' 16 [ f'' ] < g' c'' e'' > 4 r  |
                    r8 e' a' 16 [ b' ] c'' 8 [ b' ] e' 16 r b' r aes' r  |
                    r8 e' a' 16 [ b' ] c'' 8 b' 16 r e' r b' r r8  |
                    r8 f' 16 [ e' ] d' 8 [ f' e' c' ] e' [ a' ]  |
%% 10
                    aes' 8 d'' 4 a' 8 [ e' ] a' 4 aes' 8  |
                    a' 8 r r4 < c' e' a' > 8. r16 r8 a'  |
                    < aes' b' e'' > 8 r r4 < aes b e' > 8. r16 r8 e'  |
                    < c' e' a' > 8 r r c'' d'' 16 r g' 4 f' 8  |
                    e' 16 [ g' ] c'' 8 [ g' 16 c'' ] e'' 8 d'' 16 r g' 4 f' 8  |
%% 15
                    e' 16 [ g' a' b' ] c'' 8 r a' [ < f' a' > < c' a' > 16 ] a' [ b' ] c''  |
                    b' 4 f' 8. [ d' 8 b' 16 c'' d'' ] c'' 4 g' 8. [ e' 8 c'' 16 d'' e'' ]  |
                    d'' 4 g' 8. [ d' 8 d'' 16 e'' f'' ] e'' 4 g' 8. [ e' 8 e'' 16 f'' g'' ]  |
                    < g' d'' > 4 r8 b' < g' c'' > g' 4 b' g' 8  |
                    < g' c'' > 8 g' 4 g'' 8 [ b' ] < e' c'' > 8. r16 r4  |
%% 20
                    < c' e' a' > 8. r16 r8 a' < aes' b' e'' > r r4  |
                    < aes b e' > 8. r16 r8 e' < c' e' a' > r r4  |
                    r8 a' 16 r e'' r e' r < e' b' > 8 r r4  |
                    r8 e' 16 r c'' r e' r < e' aes' > 8. r16 r4  |
                    < a' cis'' e'' > 4 r8 e'' 16 [ g'' ] < a' d'' f'' > 4 r  |
%% 25
                    < g' b' d'' > 4 r8 d'' 16 [ f'' ] < g' c'' e'' > 4 r  |
                    r8 e' a' 16 [ b' ] c'' 8 [ b' ] e' 16 r b' r aes' r  |
                    r8 e' a' 16 [ b' ] c'' 8 b' 16 r e' r b' r r8  |
                    r8 f' 16 [ e' ] d' 8 [ f' e' c' ] e' [ a' ]  |
                    aes' 8 d'' 4 a' 8 [ e' ] a' 4 aes' 8  |
%% 30
                    a' 8 r r4 < c' e' a' > 8. r16 r8 a'  |
                    < aes' b' e'' > 8 r r4 < aes b e' > 8. r16 r8 e'  |
                    < c' e' a' > 8 r r c'' d'' 16 r g' 4 f' 8  |
                    e' 16 [ g' ] c'' 8 [ g' 16 c'' ] e'' 8 d'' 16 r g' 4 f' 8  |
                    e' 16 [ g' a' b' ] c'' 8 r a' [ < f' a' > < c' a' > 16 ] a' [ b' ] c''  |
%% 35
                    b' 4 f' 8. [ d' 8 b' 16 c'' d'' ] c'' 4 g' 8. [ e' 8 c'' 16 d'' e'' ]  |
                    d'' 4 g' 8. [ d' 8 d'' 16 e'' f'' ] e'' 4 g' 8. [ e' 8 e'' 16 f'' g'' ]  |
                    < g' d'' > 4 r8 b' < g' c'' > g' 4 b' g' 8  |
                    < g' c'' > 8 g' 4 g'' 8 [ b' ] < e' c'' > 8. r16 r4  |
                    < e' g' c'' > 8. r16 r8 c'' < b' d'' g'' > r r4  |
%% 40
                    < b d' g' > 8. r16 r8 g' < e' g' c'' > r r4  |
                    r8 e'' c'' g'' 16 r < b' d'' > 8. r16 r4  |
                    r8 g' c'' 16 [ d'' ] e'' 4.. a' 4. a' 8  |
                    r8 a' d'' 16 [ e'' ] f'' 4.. b' 4. b' 8  |
                    r8 b' e'' 16 [ f'' ] g'' 4.. c'' 4. c'' 8  |
%% 45
                    r8 c'' f'' 16 [ g'' ] a'' 8 [ g'' ] fis'' r4  |
                    r8 e' [ fis' b ] g' [ b ] a' b _~  |
                    < g' b > 8. r8 r b' < g' e'' > 8. r16 r8 b'  |
                    ees' 16 [ fis' ] e' ees' 4 r16 fis' [ a' ] g' fis' 4 r16  |
                    a' 16 [ c'' ] b' a' 8. < a' ees'' > 8 < g' e'' > 16 r < e' b' > r < e' c'' > r < ees' b' > r  |
%% 50
                    e' 8 [ f' e' d' ] < c' e' a' > 8. r16 r8 a'  |
                    < aes' b' e'' > 8 r r4 < aes b e' > 8. r16 r8 e'  |
                    < c' e' a' > 8 r r4 r8 a' e'' e' 16 r  |
                    < e' b' > 8. r16 r4 r8 e' [ c'' e' ]  |
                    e' 8. r16 r4 r8 a' 16 r c'' r e'' r  |
%% 55
                    r8 f' 16 [ g' ] a' [ b' ] c'' d'' 8. g' 16 r b' r d'' r  |
                    r8 e' 16 [ f' ] g' [ a' ] b' c'' 8. f' 16 r a' r c'' r  |
                    r8 d' 16 [ e' ] f' [ g' ] a' b' 8 a' 16 aes' 8 r4  |
                    r8 e' 16 [ b' ] d'' [ b' ] c'' 8 r e' 16 [ b' ] d'' [ b' ] c'' 8  |
                    r8 e' 16 r < b' b' > 8 e'' r c'' 16 r < a' a' > 8 d''  |
%% 60
                    < aes' b' > 8 r r < e' b' > < e' a' > 16 r e' r f' r e' r  |
                    r8 c' e' 8. [ a' 16 ] c'' < e' b' > 8 d' 8. e' [ b' 8 d'' 16 ]  |
                    < e' c'' > 8 e' 8. [ a' c'' 8 ] e'' 16 < a' d'' > 8 aes' 8. b' [ d'' 8 f'' 16 ]  |
                    < b' e'' > 8 a' 8. [ c'' e'' 8 ] a'' 16 < a' d'' > 8. r16 < a' d'' > 8. r16  |
                    < f' b' d'' > 8. r16 r4 < a' d'' > 8. r16 < a' d'' > 8. r16  |
%% 65
                    < e' b' d'' > 8. r16 r4 aes' 16 [ b' a' c'' ] b' [ d'' c'' b' ]  |
                    e'' 16 r < e' a' > r < d' f' > r < b e' > r < c' e' a' > 8. r16 r8 a'  |
                    < aes' b' e'' > 8. r16 r8 e'' < c'' e'' a'' > 8. r16 r8 < c'' e'' a'' >  |
                    < b' f'' a'' > 8. r16 < b' e'' aes'' > 8. r16 < c'' e'' a'' > 4.. r16  |
                    r8 < e' g' c'' > 8. r16 r8 c'' < b' d'' g'' > r4  |
%% 70
                    r8 < b d' g' > 8. r16 r8 g' < e' g' c'' > r4  |
                    r4 e'' 8 [ c'' ] g'' 16 r < b' d'' > 8. r16 r8  |
                    r4 g' 8 [ c'' 16 d'' ] e'' 4.. a' 4  |
                    < a' a' > 8 r a' d'' 16 [ e'' ] f'' 4.. b' 4  |
                    < b' b' > 8 r b' e'' 16 [ f'' ] g'' 4.. c'' 4  |
%% 75
                    < c'' c'' > 8 r c'' f'' 16 [ g'' ] a'' 8 [ g'' ] fis'' r  |
                    r4 e' 8 [ fis' b g' ] b [ a' ]  |
                    b 4 g' 8. r16 r8 b' < g' e'' > 8. r16 r8  |
                    b' 8 [ ees' 16 fis' ] e' ees' 4 r16 fis' [ a' ] g' fis' 8. _~  |
                    fis' 16 r a' [ c'' b' ] a' 8. [ < a' ees'' > 8 < g' e'' > 16 ] r < e' b' > r < e' c'' > r  |
%% 80
                    < ees' b' > 16 r e' 8 [ f' e' d' ] < c' e' a' > 8. r16 r8  |
                    a' 8 [ < aes' b' e'' > ] r4 r8 < aes b e' > 8. r16 r8  |
                    e' 8 [ < c' e' a' > ] r4 r a' 8 [ e'' ]  |
                    e' 16 r < e' b' > 8. r16 r8 r4 e' 8 [ c'' ]  |
                    e' 8 e' 8. r16 r8 r4 a' 16 r c'' r  |
%% 85
                    e'' 16 r r8 f' 16 [ g' ] a' [ b' ] c'' d'' 8. g' 16 r b' r  |
                    d'' 16 r r8 e' 16 [ f' ] g' [ a' ] b' c'' 8. f' 16 r a' r  |
                    c'' 16 r r8 d' 16 [ e' ] f' [ g' ] a' b' 8 a' 16 aes' 8 r  |
                    r4 e' 16 [ b' d'' b' ] c'' 8 r e' 16 [ b' ] d'' [ b' ]  |
                    c'' 8 r e' 16 r < b' b' > 8 e'' r c'' 16 r < a' a' > 8 _~  |
%% 90
                    d'' 8 [ a' 16 r < aes' b' > 8 ] r4 < e' b' > 8 < e' a' > 16 r e' r f' r  |
                    e' 16 r r8 c' e' 8. [ a' 16 ] c'' < e' b' > 8 d' 8. e' 8 _~  |
                    b' 8 e' 16 d'' < e' c'' > 8 e' 8. a' [ c'' 8 e'' 16 ] < a' d'' > 8 aes' 8. [ b' 8 _~ ]  |
                    d'' 8 b' 16 f'' < b' e'' > 8 a' 8. c'' [ e'' 8 a'' 16 ] < a' d'' > 8. r16 < a' d'' > 8 _~  |
                    < a' d'' > 16 r < f' b' d'' > 8. r16 r8 r < a' d'' > 8. r16 < a' d'' > 8 _~  |
%% 95
                    < a' d'' > 16 r < e' b' d'' > 8. r16 r8 r aes' 16 b' [ a' c'' b' ] d''  |
                    c'' 16 [ b' ] e'' r < e' a' > r < d' f' > r < b e' > r < c' e' a' > 8. r16 r8  |
                    a' 8 < aes' b' e'' > 8. r16 r8 e'' < c'' e'' a'' > 8. r16 r8  |
                    < c'' e'' a'' > 8 < b' f'' a'' > 8. r16 < b' e'' aes'' > 8. r16 < c'' e'' a'' > 4. _~  |
                    < c'' e'' a'' > 16  |
                    \bar "|."
                } % Voice
            >> % Staff ends

            \context Staff = "track 3, Piano" << 
                \set Staff.instrumentName = \markup { \center-column { "Piano " } }
                \set Staff.midiInstrument = "Acoustic Grand Piano"
                \set Score.skipBars = ##t
                \set Staff.printKeyCancellation = ##f
                \new Voice \global
                \new Voice \globalTempo
                \set Staff.autoBeaming = ##f % turns off all autobeaming

                \context Voice = "voice 3" {
                    % Segment: Acoustic Grand Piano
                    \override Voice.TextScript #'padding = #2.0
                    \override MultiMeasureRest #'expand-limit = 1
                    \partial 64*8
                    \once \override Staff.TimeSignature #'style = #'default 
                    \time 4/4
                    
%% 0
                    \clef "bass"
                    r8  |
                    a, 8. r16 r4 r8 e e, 8. r16  |
                    e, 8. r16 r4 r8 a a, 8. r16  |
                    a 8. r16 a 8. r16 aes 8 e 16 r e, 8. r16  |
                    a 8. r16 a 8. r16 e 8 e 16 r e, 8. r16  |
%% 5
                    r8 g 16 r a, 8. r16 r8 f, 16 r d, 8. r16  |
                    r8 f 16 r g, 8. r16 r8 e, 16 r c, 8. r16  |
                    a, 8. r16 a, 8. r16 e 8. r16 e 8. r16  |
                    a, 8. r16 a 8. r16 e 8. r16 e, 8. r16  |
                    aes, 8. r16 aes, 8. r16 a, 8. r16 r8 a  |
%% 10
                    e 8. r16 f 8. r16 e 8. r16 e, 8. r16  |
                    a, 8 [ b, c b, ] a, 8. r16 r4  |
                    r8 e e, 8. r16 e, 8. r16 r4  |
                    r8 a a, 8. r16 b, 8. r16 g, 8. r16  |
                    c 8 [ d e c ] b, 8. r16 g, 8. r16  |
%% 15
                    c 8 [ d e c ] f 8. r16 r4  |
                    g 8. r16 r4 a 8. r16 r4  |
                    b 8. r16 r4 c' 8. r16 r4  |
                    b 8 [ a b g ] c' 8. r16 g 8. r16  |
                    c 8. r16 < g, g > 8. r16 < e, e > 8. r16 r4  |
%% 20
                    a, 8. r16 r4 r8 e e, 8. r16  |
                    e, 8. r16 r4 r8 a a, 8. r16  |
                    a 8. r16 a 8. r16 aes 8 e 16 r e, 8. r16  |
                    a 8. r16 a 8. r16 e 8 e 16 r e, 8. r16  |
                    r8 g 16 r a, 8. r16 r8 f, 16 r d, 8. r16  |
%% 25
                    r8 f 16 r g, 8. r16 r8 e, 16 r c, 8. r16  |
                    a, 8. r16 a, 8. r16 e 8. r16 e 8. r16  |
                    a, 8. r16 a 8. r16 e 8. r16 e, 8. r16  |
                    aes, 8. r16 aes, 8. r16 a, 8. r16 r8 a  |
                    e 8. r16 f 8. r16 e 8. r16 e, 8. r16  |
%% 30
                    a, 8 [ b, c b, ] a, 8. r16 r4  |
                    r8 e e, 8. r16 e, 8. r16 r4  |
                    r8 a a, 8. r16 b, 8. r16 g, 8. r16  |
                    c 8 [ d e c ] b, 8. r16 g, 8. r16  |
                    c 8 [ d e c ] f 8. r16 r4  |
%% 35
                    g 8. r16 r4 a 8. r16 r4  |
                    b 8. r16 r4 c' 8. r16 r4  |
                    b 8 [ a b g ] c' 8. r16 g 8. r16  |
                    c 8. r16 < g, g > 8. r16 < e, e > 8. r16 r4  |
                    c 8. r16 r4 r8 d g, 8. r16  |
%% 40
                    g 8. r16 r4 r8 c' c 8. r16  |
                    c 8. r16 c 8. r16 g, 8 g 16 r g, 8. r16  |
                    e 2 c' f 4.. f' 8. r16  |
                    f 2 d' g 4.. g' 8. r16  |
                    g 2 e' a 4.. a' 8. r16  |
%% 45
                    a 4.. c' 8. r16 < b, b > 8 b g e  |
                    b, 8 r b, r cis r ees r  |
                    e 4 b 8 [ e' ] r e 4 b 8 e' r  |
                    fis 8 [ b ] b, 8. r16 r8 b b, 8. r16  |
                    r8 b b, 8. r16 e r g r a r b r  |
%% 50
                    e 16 r d 8 [ c ] b, a, 8. r16 r4  |
                    r8 e e, 8. r16 e, 8. r16 r4  |
                    r8 a a, 8. r16 a 8. r16 a 8. r16  |
                    aes 8 [ e ] e, 8. r16 a 8. r16 a 8. r16  |
                    aes 8 [ e ] e, 8. r16 a, 4.. r16  |
%% 55
                    d 4.. r16 g, 4.. r16  |
                    c 4.. r16 f, 4.. r16  |
                    b, 4.. r16 e, 8 e 16 r e, 8. r16  |
                    r8 e 16 r e, 8. r16 r8 e 16 r e, 8. r16  |
                    b 8. r16 aes 8. r16 a 8. r16 f 8. r16  |
%% 60
                    e 8. r16 aes, 8. r16 a, r c r d r e r  |
                    a, 8. r16 r4 aes, 8. r16 r4  |
                    a, 8. r16 r4 b, 8. r16 r4  |
                    c 8. r16 a 8. r16 f 8. r16 f 8. r16  |
                    aes, 8. r16 r4 f 8. r16 f 8. r16  |
%% 65
                    aes, 8. r16 r4 r aes, 8. r16  |
                    a, 16 r c r d r e r a, 8. r16 r4  |
                    e, 8. r16 r4 r8 a [ a, a ]  |
                    d, 8. r16 e, 8. r16 < a,, a, > 4.. r16  |
                    r8 c 8. r16 r8 r4 d 8 g, _~  |
%% 70
                    g, 16 r g 8. r16 r8 r4 c' 8 c _~  |
                    c 16 r c 8. r16 c 8. r16 g, 8 [ g 16 ] r g, 8 _~  |
                    g, 16 r e 2 c' f 4. _~ f' 8 _~  |
                    < f f' > 16 r f 2 d' g 4. _~ g' 8 _~  |
                    < g g' > 16 r g 2 e' a 4. _~ a' 8 _~  |
%% 75
                    < a a' > 16 r a 4.. c' 8. r16 < b, b > 8 [ b ] g  |
                    e 8 [ b, ] r b, r cis r ees  |
                    r8 e 4 b 8 e' r e 4 b 8 e'  |
                    r8 fis b b, 8. r16 r8 b b, _~  |
                    b, 16 r r8 b b, 8. r16 e r g r a r  |
%% 80
                    b 16 r e r d 8 [ c ] b, a, 8. r16 r8  |
                    r4 e 8 e, 8. r16 e, 8. r16 r8  |
                    r4 a 8 a, 8. r16 a 8. r16 a 8 _~  |
                    a 16 r aes 8 e e, 8. r16 a 8. r16 a 8 _~  |
                    a 16 r aes 8 e e, 8. r16 a, 4. _~  |
%% 85
                    a, 16 r d 4.. r16 g, 4. _~  |
                    g, 16 r c 4.. r16 f, 4. _~  |
                    f, 16 r b, 4.. r16 e, 8 [ e 16 ] r e, 8 _~  |
                    e, 16 r r8 e 16 r e, 8. r16 r8 e 16 r e, 8 _~  |
                    e, 16 r b 8. r16 aes 8. r16 a 8. r16 f 8 _~  |
%% 90
                    f 16 r e 8. r16 aes, 8. r16 a, r c r d r  |
                    e 16 r a, 8. r16 r8 r aes, 8. r16 r8  |
                    r8 a, 8. r16 r8 r b, 8. r16 r8  |
                    r8 c 8. r16 a 8. r16 f 8. r16 f 8 _~  |
                    f 16 r aes, 8. r16 r8 r f 8. r16 f 8 _~  |
%% 95
                    f 16 r aes, 8. r16 r8 r4 r8 aes, _~  |
                    aes, 16 r a, r c r d r e r a, 8. r16 r8  |
                    r8 e, 8. r16 r8 r4 a 8 [ a, ]  |
                    a 8 d, 8. r16 e, 8. r16 < a,, a, > 4. _~  |
                    < a,, a, > 16  |
                    \bar "|."
                } % Voice
            >> % Staff (final) ends
        >> % GrandStaff (final) 1

    >> % notes

    \layout {
        indent = 3.0\cm
        short-indent = 1.5\cm
        \context { \Staff \RemoveEmptyStaves }
        \context { \GrandStaff \accepts "Lyrics" }
    }
%     uncomment to enable generating midi file from the lilypond source
%         \midi {
%         } 
} % score
