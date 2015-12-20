#-*-coding:utf-8-*-
import sys

SLICE_N = 15

class Question:
    def __init__(self, item, idx):
        self.item = item
        self.url = 'http://dn-goodview.qbox.me/' + item
        self.idx = idx

    def resp(self):
        template = \
u'''
  <Question> 
    <QuestionIdentifier>%s</QuestionIdentifier>  
    <IsRequired>true</IsRequired>
    <QuestionContent> 
      <Text>Progress %d/%d: %s</Text>  
      <Binary> 
        <MimeType> 
          <Type>image</Type>  
          <SubType>jpg</SubType> 
        </MimeType>  
        <DataURL>%s</DataURL>  
        <AltText>Focus on the viewpoint of this photo</AltText> 
      </Binary> 
    </QuestionContent>  
    <AnswerSpecification> 
      <SelectionAnswer> 
        <StyleSuggestion>radiobutton</StyleSuggestion>  
        <Selections> 
          <Selection> 
            <SelectionIdentifier>good[%s]</SelectionIdentifier>  
            <Text>I think the viewpoint is good</Text> 
          </Selection>  
          <Selection> 
            <SelectionIdentifier>bad[%s]</SelectionIdentifier>  
            <Text>I think the viewpoint is bad</Text> 
          </Selection>
        </Selections> 
      </SelectionAnswer> 
    </AnswerSpecification> 
  </Question>
'''
        return template % (self.item, self.idx+1, SLICE_N, self.item, self.url, self.item, self.item)


class Survey:
    def __init__(self, identity, items):
        self.identity = identity
        self.questions = [Question(item, idx) for idx, item in enumerate(items)]

    def question_resp(self):
        template = \
u'''<?xml version="1.0" encoding="utf-8"?>
<QuestionForm xmlns="http://mechanicalturk.amazonaws.com/AWSMechanicalTurkDataSchemas/2005-10-01/QuestionForm.xsd">
  <Overview>
    <Title>Evaluate these photos' viewpoints</Title>
    <Text>Feel free to give us your opinions about these photos, focus on its' viewpoints</Text>
    <Text>Each HIT has %d pictures :)</Text>
  </Overview>
  
  %s
</QuestionForm>
'''
        strs = [q.resp() for q in self.questions]
        return template % (SLICE_N, '\n'.join(strs))

    def property_resp(self):
        template = \
u'''title:%s
description:We need your opinions whether the viewpoints of photos are good or bad.
keywords:viewpoint, evaluation 
reward:0.01
assignments:20
annotation:sample#command

######################################
## HIT Timing Properties
######################################

# this Assignment Duration value is 60 * 60 = 1 hour
assignmentduration:3600

# this HIT Lifetime value is 60*60*24*3 = 3 days
hitlifetime:259200

# this Auto Approval period is 60*60*24*15 = 15 days
autoapprovaldelay:1296000'''
        return template % self.identity


if __name__ == '__main__':
    idx = 0
    items = []
    #with open('list.txt', 'r') as inf:
    for line in sys.stdin:
        line = line.decode('utf-8').strip()
        item = line.split()[0]
        items.append(item)
    for idx in xrange(0, len(items), SLICE_N):
        end_idx = idx + SLICE_N
        if end_idx >= len(items):
            break
        identity = 'goodview_%d-%d' % (idx, end_idx - 1)
        a = Survey(identity, items[idx:end_idx])
        with open(identity + '.properties', 'w') as f:
            f.write(a.property_resp().encode('utf-8'))
        with open(identity + '.question', 'w') as f:
            f.write(a.question_resp().encode('utf-8'))
        with open(identity + '.input', 'w') as f:
            f.write('1\ntest is good')
