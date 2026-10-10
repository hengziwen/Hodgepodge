"""Record real montage end events independently of Slate sampling frequency."""
import unreal,builtins

@unreal.uclass()
class HodgeMovementMontageValidationObserver(unreal.Object):
    pawn_key=unreal.uproperty(str)

    @unreal.ufunction(params=[unreal.AnimMontage,bool])
    def receive_end(self,montage,interrupted):
        context=getattr(builtins,'HODGE_REACTION_TEST',None)
        if context and montage:
            context.setdefault('montage_end_events',{}).setdefault(self.get_editor_property('pawn_key'),[]).append(
                {'time':unreal.GameplayStatics.get_time_seconds(context['world']), 'montage':str(montage.get_name()),'interrupted':interrupted})

def unbind(context):
    for anim,observer in context.get('montage_observers',[]):
        try:
            if unreal.SystemLibrary.is_valid(anim):anim.on_montage_ended.remove_function(observer,'receive_end')
        except (TypeError,RuntimeError):pass
    context['montage_observers']=[]

def bind(pawns,context):
    unbind(context);context['montage_end_events']={};seen=set()
    for pawn in pawns:
        key=pawn.get_path_name()
        if key in seen:continue
        seen.add(key);anim=pawn.mesh.get_anim_instance();assert anim
        observer=unreal.new_object(HodgeMovementMontageValidationObserver)
        observer.set_editor_property('pawn_key',key)
        anim.on_montage_ended.add_function(observer,'receive_end')
        context['montage_observers'].append((anim,observer))
