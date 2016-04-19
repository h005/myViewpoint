#-*-coding:utf-8-*-
import sys
import random
from random import randint

ID_PREFIX= 'kxm_'
SLICE_N = 30
REWARD = 0.02
ASSIGNMENTS = 20

class Question:
    def __init__(self, item, answer_id, idx):
        self.item = item
        self.answer_id = answer_id
        self.url = 'http://7xoro6.com1.z0.glb.clouddn.com/' + item
        self.idx = idx

    def resp(self):
        template = \
u'''
  <Question> 
    <QuestionIdentifier>{0}</QuestionIdentifier>  
    <IsRequired>true</IsRequired>
    <QuestionContent> 
      <Text>Progress {1}/{2}: {0}</Text>  
      <Binary> 
        <MimeType> 
          <Type>image</Type>  
          <SubType>jpg</SubType> 
        </MimeType>  
        <DataURL>{3}</DataURL>  
        <AltText>Focus on the viewpoint of this photo</AltText> 
      </Binary> 
    </QuestionContent>  
    <AnswerSpecification> 
      <SelectionAnswer> 
        <StyleSuggestion>radiobutton</StyleSuggestion>  
        <Selections> 
          <Selection> 
            <SelectionIdentifier>1[{4}]</SelectionIdentifier>  
            <Text>1: I think the viewpoint is poor</Text> 
          </Selection>
          <Selection> 
            <SelectionIdentifier>2[{4}]</SelectionIdentifier>  
            <Text>2: I think the viewpoint is fair</Text> 
          </Selection> 
          <Selection> 
            <SelectionIdentifier>3[{4}]</SelectionIdentifier>  
            <Text>3: I think the viewpoint is satisfactory</Text> 
          </Selection> 
          <Selection> 
            <SelectionIdentifier>4[{4}]</SelectionIdentifier>  
            <Text>4: I think the viewpoint is good</Text> 
          </Selection>  
          <Selection> 
            <SelectionIdentifier>5[{4}]</SelectionIdentifier>  
            <Text>5: I think the viewpoint is excellent</Text> 
          </Selection> 
        </Selections> 
      </SelectionAnswer> 
    </AnswerSpecification> 
  </Question>
'''
        # answer_id在陷阱图片中，和item会有不同
        return template.format(self.item, self.idx+1, SLICE_N, self.url, self.answer_id)


class Survey:
    def __init__(self, identity, items, traps):
        self.identity = identity
        self.questions = [Question(item, item, idx) for idx, item in enumerate(items)]
        for item in traps:
            self.questions.insert(randint(0,len(self.questions)), Question(item, item + '*', idx))

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
        return template % (SLICE_N + 1, '\n'.join(strs))

    def property_resp(self):
        template = \
u'''title:%s
description:We need your opinions whether the viewpoints of photos are good or bad.
keywords:viewpoint, evaluation 
reward:%f
assignments:%d
annotation:sample#command

######################################
## HIT Timing Properties
######################################

# this Assignment Duration value is 60 * 60 = 1 hour
assignmentduration:3600

# this HIT Lifetime value is 60*60*24*5 = 5 days
hitlifetime:432000

# this Auto Approval period is 60*60*24*15 = 15 days
autoapprovaldelay:1296000'''
        return template % (self.identity, REWARD, ASSIGNMENTS)


if __name__ == '__main__':
    import optparse
    usage = 'Usage: %prog --prefix=kxm_ [--matrix-in] < kxm.txt'  
    parser = optparse.OptionParser(usage = usage)
    parser.add_option('--matrix-in', dest='matrix_in', default=False, action='store_true', help='using matrix format')
    parser.add_option('--prefix', dest='prefix', default='kxm_', help='survey prefix')
    parser.add_option('--bad-item-file', dest='bad_item_f', help='the file which has bad item names')
    (options, args) = parser.parse_args()

    ID_PREFIX = options.prefix

    bad_items = []
    if options.bad_item_f:
        with open(options.bad_item_f, 'r') as f:
            for line in f:
                bad_items.append(line.strip())

    idx = 0
    items = []
    if options.matrix_in:
        # matrix format
        while True:
            first = sys.stdin.readline()
            if not first:
                break
            first = first.strip()
            for i in xrange(8):
                sys.stdin.readline()
            items.append(first)
    else:
        # list format
        for line in sys.stdin:
            line = line.decode('utf-8').strip()
            item = line.split()[0]
            items.append(item)

    for idx in xrange(0, len(items), SLICE_N):
        end_idx = idx + SLICE_N
        if end_idx >= len(items):
            # 如果最后一批图片少于SLICE_N张，则放弃生成该批图片
            break
        identity = '%s%d-%d' % (ID_PREFIX, idx, end_idx - 1)
        current_items = items[idx:end_idx]
        # add item for evaluation
        traps = []
        if len(bad_items) > 0:
            traps.append(random.choice(bad_items))

        a = Survey(identity, current_items, traps)
        with open(identity + '.properties', 'w') as f:
            f.write(a.property_resp().encode('utf-8'))
        with open(identity + '.question', 'w') as f:
            f.write(a.question_resp().encode('utf-8'))
        with open(identity + '.input', 'w') as f:
            f.write('1\ntest is good')
